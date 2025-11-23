// so this file's gonna contain the train of bpe, as in finding the merge id, recomputing if its feasible merging it or just go to the next one

#include <bits/stdc++.h>

using namespace std;

int get_utf8_char_len(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

// build vocab from unique utf8 chars in text
int build_initial_vocab(const string& filename, unordered_map<string, int>& vocab){
  ifstream f(filename);
  if(!f.is_open()){
    cerr<<"Error: unable to open file "<<filename<<endl;
    exit(1);
  }

  unordered_set<string> unique_chars;
  string line;

  while(getline(f, line)){
    for(size_t i=0; i<line.size();){
      int len = get_utf8_char_len(line[i]);
      if(i + len <= line.size()){
        string utf8_char = line.substr(i, len);
        unique_chars.insert(utf8_char);
        i += len;
      } else {
        i++;
      }
    }
    unique_chars.insert("\n");
  }

  int id = 0;
  for(const auto& ch : unique_chars){
    vocab[ch] = id++;
  }

  return id;
}

vector<int> encode_text_to_ids(const string& filename, const unordered_map<string, int>& vocab){
  ifstream f(filename);
  if(!f.is_open()){
    cerr<<"Error: unable to open file "<<filename<<endl;
    exit(1);
  }

  vector<int> ids;
  string line;
  while(getline(f, line)){
    for(size_t i=0; i<line.size();){
      int len = get_utf8_char_len(line[i]);
      if(i + len <= line.size()){
        string utf8_char = line.substr(i, len);

        auto it = vocab.find(utf8_char);
        if(it != vocab.end()){
          ids.push_back(it->second);
        }
        i += len;
      } else {
        i++;
      }
    }
    // add newline token
    auto it = vocab.find("\n");
    if(it != vocab.end()){
      ids.push_back(it->second);
    }
  }

  return ids;
}

map<pair<int, int>, int> get_stats(const vector<int>& ids){
  map<pair<int, int>, int> freq;
  for(int i=0;i<ids.size()-1;i++){
    freq[{ids[i], ids[i+1]}]++;
  }
  return freq;
}

// wanna merge the new ids incoming now, but stuck in this for around 10 mins thinking how to it
vector<int> merge_ids(const vector<int>& ids, pair<int, int> best_pair, int new_id){
  vector<int> res_ids;
  res_ids.reserve(ids.size());

  int i=0;
  while(i<ids.size()){
    if(i<ids.size()-1 && ids[i] == best_pair.first && ids[i+1] == best_pair.second){
      res_ids.push_back(new_id);
      i+=2;
    }else{
      res_ids.push_back(ids[i]);
      i++;
    }
  }

  return res_ids;
}

int main(){
  unordered_map<string, int> vocab;

  int next_token_id = build_initial_vocab("shakespeare.txt", vocab);
  cout<<"initial vocab size: "<<next_token_id<<endl;

  vector<int> train_ids = encode_text_to_ids("shakespeare.txt", vocab);
  cout<<"first token id: "<<train_ids[0]<<endl;
  cout<<"initial token count: "<<train_ids.size()<<endl;

  int num_merges = 50;

  vector<pair<pair<int, int>, int>> ordered_merges;

  for(int i=0;i<num_merges;i++){
    map<pair<int, int>, int> stats = get_stats(train_ids);


    // priority_queue<pair<int, int>, int>>> pq;
    priority_queue<pair<int, pair<int, int>>> pq;

    for(auto& p : stats){
      pq.push({p.second, p.first});
    }

    if(pq.empty()) break;

    pair<int, pair<int, int>> best = pq.top();
    int count = best.first;
    pair<int, int> new_id_to_merge = best.second;

    if(count<2){
      cout<<"doesn't matter, the max frequency is 1"<<endl;
      break;
    }

    cout << "Step " << (i + 1) << "/" << num_merges
             << " | Merging IDs (" << new_id_to_merge.first << ", " << new_id_to_merge.second << ")"
             << " -> New ID: " << next_token_id
             << " | Count: " << count << endl;

    ordered_merges.push_back({new_id_to_merge, next_token_id});

    train_ids = merge_ids(train_ids, new_id_to_merge, next_token_id);

    next_token_id++;
  }

  cout<<"the training is complete now!!"<<endl;
  cout<<"final vocab size: "<<next_token_id<<endl;
  cout<<"final compressed token count: "<<train_ids.size()<<endl;

  ofstream merge_file("merges.txt");

  for(auto& p: ordered_merges){
    merge_file<<p.first.first<<" "<<p.first.second<<" "<<p.second<<endl;
  }

  merge_file.close();

  cout<<"merges saved to merge.txt"<<endl;
  return 0;
}