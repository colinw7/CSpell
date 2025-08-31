#ifndef CSpellTrie_H
#define CSpellTrie_H

#include <CSpellTrie.h>
#include <CTrie.h>

#include <map>
#include <string>

class CSpellTrie {
 public:
  CSpellTrie();
 ~CSpellTrie();

  bool load();

  bool checkWord(const std::string &word) const;

  bool checkStartWord(const std::string &word) const;

  void complete(const std::string &word, std::vector<std::string> &strs) const;

 private:
  struct WordFlags {
    WordFlags() { }

    bool d_flag  { false };
    bool e_flag  { false };
    bool g_flag  { false };
    bool h_flag  { false };
    bool j_flag  { false };
    bool m_flag  { false };

    bool p_flag  { false };
    bool r_flag  { false };

    bool t_flag  { false };
    bool v_flag  { false };
    bool x_flag  { false };
    bool y_flag  { false };
    bool z_flag  { false };

    bool n_flag  { false };
    bool n1_flag { false };
    bool n2_flag { false };
    bool n3_flag { false };

    bool s_flag  { false };
    bool s1_flag { false };
    bool s2_flag { false };
    bool s3_flag { false };
    bool s4_flag { false };
    bool s5_flag { false };

    bool ion_flag     { false };
    bool ication_flag { false };
    bool en_flag      { false };
    bool ing_flag     { false };
    bool er_flag      { false };
  };

  void addDWord(const std::string &word, const WordFlags &flags=WordFlags());
  void addEWord(const std::string &word, const WordFlags &flags=WordFlags());
  void addGWord(const std::string &word, WordFlags &flags);
  void addHWord(const std::string &word, const WordFlags &flags=WordFlags());
  void addRWord(const std::string &word, WordFlags &flags);
  void addTWord(const std::string &word, const WordFlags &flags=WordFlags());
  void addYWord(const std::string &word, const WordFlags &flags=WordFlags());

  void addN1Word(const std::string &word, WordFlags &flags);
  void addN2Word(const std::string &word, WordFlags &flags);
  void addN3Word(const std::string &word, WordFlags &flags);

  void addIonsWord    (const std::string &word, const WordFlags &flags=WordFlags());
  void addIcationsWord(const std::string &word, const WordFlags &flags=WordFlags());
  void addEnsWord     (const std::string &word, const WordFlags &flags=WordFlags());
  void addIngsWord    (const std::string &word, const WordFlags &flags=WordFlags());
  void addErsWord     (const std::string &word, const WordFlags &flags=WordFlags());
  void addS1Word      (const std::string &word, const WordFlags &flags=WordFlags());
  void addS2Word      (const std::string &word, const WordFlags &flags=WordFlags());
  void addS3Word      (const std::string &word, const WordFlags &flags=WordFlags());
  void addS4Word      (const std::string &word, const WordFlags &flags=WordFlags());
  void addS5Word      (const std::string &word, const WordFlags &flags=WordFlags());

  void addBaseWord (const std::string &word, const WordFlags &flags=WordFlags());
  void addExtraWord(const std::string &origWord, const std::string &word);

  bool isVowel(char c) const;

 private:
  using WordFlagsMap = std::map<std::string, WordFlags>;
  using Words        = std::set<std::string>;

  CTrie        trie_;
  WordFlagsMap wordFlags_;
  Words        words_;
};

#endif
