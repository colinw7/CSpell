#include <CSpellTrie.h>

#include <string>
#include <iostream>

int
main(int argc, char **argv)
{
  std::string word;
  bool        start { false };
  bool        complete { false };

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = std::string(&argv[i][1]);

      if      (arg == "h" || arg == "help")
        std::cerr << "CSpellTrie [-start|-complete] <word>\n";
      else if (arg == "start")
        start = true;
      else if (arg == "complete")
        complete = true;
    }
    else
      word = argv[i];
  }

  CSpellTrie trie;

  if (! trie.load())
    return 1;

  if (word == "")
    return 0;

  if      (start) {
    auto rc = trie.checkStartWord(word);
    std::cerr << (rc ? "Valid" : "Invalid") << "\n";
  }
  else if (complete) {
    std::vector<std::string> strs;
    trie.complete(word, strs);
    for (const auto &s : strs)
      std::cerr << " " << s;
    std::cerr << "\n";
  }
  else {
    auto rc = trie.checkWord(word);
    std::cerr << (rc ? "Valid" : "Invalid") << "\n";
  }

  return 0;
}
