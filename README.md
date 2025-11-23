# BPE Tokenizer in C++

A from-scratch implementation of Byte Pair Encoding (BPE) tokenizer in C++, following the BPE algorithm for subword tokenization.

## Files

### `build_vocab.cpp`
Builds the initial vocabulary from unique UTF-8 characters. Reads `shakespeare.txt` and `emojis.txt`, extracts all unique UTF-8 characters, and maps them to sequential token IDs. Outputs `unique_char_map.txt` in the format `id : character`.

### `train_bpe.cpp`
Implements the BPE training algorithm. Loads the initial vocabulary, encodes the training text into token IDs, then iteratively finds the most frequent adjacent token pairs and merges them into new tokens. Performs a configurable number of merges (default: 50) and outputs `merges.txt` containing merge rules in the format `id1 id2 new_id`.

### `tokenizer.cpp`
Main tokenizer implementation providing encode and decode functionality. Loads the vocabulary from `unique_char_map.txt` and merge rules from `merges.txt`, then applies BPE encoding to convert text into token IDs and decoding to reconstruct the original text. Handles UTF-8 characters including emojis.

## Usage

1. Build initial vocabulary: `g++ build_vocab.cpp -o build_vocab && ./build_vocab`
2. Train BPE merges: `g++ train_bpe.cpp -o train_bpe && ./train_bpe`
3. Tokenize text: `g++ tokenizer.cpp -o tokenizer && ./tokenizer`

## Output Verification

The tokenizer successfully encodes and decodes text with UTF-8 characters including emojis. Example output:
- Original: `To be, or not to be, that is the question: 🙄`
- Encoded: `584 373 547 392 420 338 547 373 514 547 353 373 387 547 387 373 547 392 420 338 547 387 470 831 387 547 357 351 547 387 470 420 547 485 428 420 351 387 357 373 353 395 547 339`
- Decoded: `To be, or not to be, that is the question: 🙄`
- Result: Decoded text matches original (round-trip encoding verified)

## References

- [Let's build the GPT Tokenizer](https://www.youtube.com/watch?v=zduSFxRajkE) - Karpathy's video on building a tokenizer
- [HuggingFace LLM Course: Byte-Pair Encoding Tokenization](https://huggingface.co/learn/llm-course/en/chapter6/5)
- [Karpathy's nanochat BPE implementation](https://github.com/karpathy/nanochat/blob/master/rustbpe/src/lib.rs)
