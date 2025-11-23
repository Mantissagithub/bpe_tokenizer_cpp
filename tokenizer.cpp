// this will be the 1st version of tokenization written in cpp, following this video by karpathy: https://www.youtube.com/watch?v=zduSFxRajkE

// so read this blog from hf: https://huggingface.co/learn/llm-course/en/chapter6/5
// and also gonna take some ref from karpathy's nanochat repo: https://github.com/karpathy/nanochat/blob/master/rustbpe/src/lib.rs

// now that we have all files ready, like the vob and the merges, the only thing remaining is the encode and decode, basically tokenize and de-tokenize

#include <bits/stdc++.h>

using namespace std;

unordered_map<string, int> vocab;
unordered_map<int, string> vocab_rev;
map<pair<int, int>, int> merges;

int get_utf8_char_len(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

void load_vocab_and_merge_thing(){
  ifstream ucm("unique_char_map.txt");
  if(!ucm.is_open()){
    cerr<<"Error: unable to open unique_char_map.txt"<<endl;
    exit(1);
  }

  string line;
  while(getline(ucm, line)) {
      size_t delim = line.find(" : ");
      if(delim != string::npos) {
          int id = stoi(line.substr(0, delim));

          string ch = line.substr(delim + 3);

          vocab[ch] = id;
          vocab_rev[id] = ch;
      }
  }
  ucm.close();

  ifstream mf("merges.txt");
  if(!mf.is_open()){
    cerr<<"Error: unable to open merges.txt"<<endl;
    exit(1);
  }

  int id1, id2, new_id;
  while (mf >> id1 >> id2 >> new_id) {
      merges[{id1, id2}] = new_id;

      string part1 = vocab_rev[id1];
      string part2 = vocab_rev[id2];
      vocab_rev[new_id] = part1 + part2;
  }
  mf.close();
}


vector<int> encode(string text){
  vector<int> ids;

  for(int i=0;i<text.size();){
    int len = get_utf8_char_len(text[i]);
    string ch = text.substr(i, len);
    if(vocab.count(ch)) ids.push_back(vocab[ch]);
    else ids.push_back(-1); // unknown as of now
    i += len;
  }

  while(true){
    if(ids.size() < 2) break;

    int best_idx = -1;
    int best_rank = INT_MAX;
    pair<int, int> best_pair = {-1, -1};

    // finding the pair with the lowest rank (earliest merge)
    for(int i=0; i<ids.size()-1; i++){
      pair<int, int> pair_key = {ids[i], ids[i+1]};
      if(merges.count(pair_key)){
        // in BPE, earlier merges have lower "rank"
        // we need to find which merge came first
        // for now, we'll use the new_id as a proxy (lower = earlier)
        int new_id = merges[pair_key];
        if(new_id < best_rank){
          best_rank = new_id;
          best_pair = pair_key;
          best_idx = i;
        }
      }
    }

    if(best_idx == -1) break;

    vector<int> new_ids;
    for(int i=0; i<ids.size(); i++){
      if(i == best_idx){
        new_ids.push_back(merges[best_pair]);
        i++;
      }else{
        new_ids.push_back(ids[i]);
      }
    }

    ids = new_ids;
  }

  return ids;
}

string decode(vector<int>& ids){
  string text = "";
  for(int id : ids){
    if(vocab_rev.count(id)){
      text += vocab_rev[id];
    }else{
      text += "<UNK>";
    }
  }
  return text;
}

int main(){
  load_vocab_and_merge_thing();

  string text = "To be, or not to be, that is the question: 🙄";
  cout<<"Original Text: "<<text<<endl;

  vector<int> encoded_ids = encode(text);
  cout<<"Encoded IDs: ";
  for(int id : encoded_ids){
    cout<<id<<" ";
  }
  cout<<endl;

  string decoded_text = decode(encoded_ids);
  cout<<"Decoded Text: "<<decoded_text<<endl;

  if(decoded_text == text){
    cout<<"Success: Decoded text matches original!"<<endl;
  }else{
    cout<<"Error: Decoded text does not match original."<<endl;
  }

  return 0;
}

// the result yayy
//  g++ tokenizer.cpp -o tokenizer
//  ./tokenizer
// Original Text: To be, or not to be, that is the question: 🙄
// Encoded IDs: 584 373 547 392 420 338 547 373 514 547 353 373 387 547 387 373 547 392 420 338 547 387 470 831 387 547 357 351 547 387 470 420 547 485 428 420 351 387 357 373 353 395 547 339
// Decoded Text: To be, or not to be, that is the question: 🙄
// Success: Decoded text matches original!