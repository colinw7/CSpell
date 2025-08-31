#include <CSpellTrie.h>
#include <CSpell.h>
#include <CFile.h>

#include <iostream>
#include <cassert>

#define Q(x) #x
#define QUOTE(x) Q(x)

CSpellTrie::
CSpellTrie()
{
  (void) CSpellInit();
}

CSpellTrie::
~CSpellTrie()
{
  CSpellTerm();
}

bool
CSpellTrie::
load()
{
  auto buildDir = std::string(QUOTE(BUILD_DIR));

  CFile file(buildDir + "/cspell.dict");

  if (! file.open(CFile::Mode::READ))
    return false;

  std::string line;
  while (file.readLine(line)) {
    WordFlags flags;

    bool skip = false;

    auto p = line.find('/');

    std::string word = line;

    if (p != std::string::npos) {
      word = word.substr(0, p);

      ++p;

      bool check_n = false, negate_n = false;
      bool check_s = false, negate_s = false;

      auto endings = line.substr(p);

      for (auto &e : endings) {
        if      (check_n) {
          check_n = false;

          if      (e == '1') {
            if (! negate_n) {
              flags.n2_flag = false;
              flags.n3_flag = false;
            }
            else
              flags.n1_flag = false;
            continue;
          }
          else if (e == '2') {
            if (! negate_n) {
              flags.n1_flag = false;
              flags.n3_flag = false;
            }
            else
              flags.n2_flag = false;
            continue;
          }
          else if (e == '3') {
            if (! negate_n) {
              flags.n1_flag = false;
              flags.n2_flag = false;
            }
            else
              flags.n3_flag = false;
            continue;
          }
          else if (e == '-') {
            negate_n = true;
            check_n  = true;
          }
        }
        else if (check_s) {
          check_s = false;

          if      (e == '1') {
            if (! negate_s) {
              flags.s2_flag = false;
              flags.s3_flag = false;
              flags.s4_flag = false;
              flags.s5_flag = false;
            }
            else
              flags.s1_flag = false;
            continue;
          }
          else if (e == '2') {
            if (! negate_s) {
              flags.s1_flag = false;
              flags.s3_flag = false;
              flags.s4_flag = false;
              flags.s5_flag = false;
            }
            else
              flags.s2_flag = false;
            continue;
          }
          else if (e == '3') {
            if (! negate_s) {
              flags.s1_flag = false;
              flags.s2_flag = false;
              flags.s4_flag = false;
              flags.s5_flag = false;
            }
            else
              flags.s3_flag = false;
            continue;
          }
          else if (e == '4') {
            if (! negate_s) {
              flags.s1_flag = false;
              flags.s2_flag = false;
              flags.s3_flag = false;
              flags.s5_flag = false;
            }
            else
              flags.s4_flag = false;
            continue;
          }
          else if (e == '4') {
            if (! negate_s) {
              flags.s1_flag = false;
              flags.s2_flag = false;
              flags.s3_flag = false;
              flags.s4_flag = false;
            }
            else
              flags.s5_flag = false;
            continue;
          }
          else if (e == '-') {
            negate_s = true;
            check_s  = true;
          }
        }

        if      (e == 'D') /* FOR "CREATED", "IMPLIED", "CROSSED" */
          flags.d_flag = true;
        else if (e == 'E') /* FOR "CREATIVE", "PREVENTIVE" */
          flags.e_flag = true;
        else if (e == 'G') /* FOR "CREATING", "FIXING" */
          flags.g_flag = true;
        else if (e == 'H') /* FOR "HUNDREDTH", "TWENTIETH" */
          flags.h_flag = true;
        else if (e == 'J') /* FOR 'ING */
          flags.j_flag = true;
        else if (e == 'M') /* FOR 'S */
          flags.m_flag = true;
        else if (e == 'N') { /* "TIGHTEN", "CREATION", "MULTIPLICATION" */
          flags.n_flag  = true;
          flags.n1_flag = true;
          flags.n2_flag = true;
          flags.n3_flag = true;

          check_n = true;
        }
        else if (e == 'P') /* FOR 'S */
          flags.p_flag = true;
        else if (e == 'R') /* FOR "LATER", "DIRTIER", "BOLDER" */
          flags.r_flag = true;
        else if (e == 'S') {
          flags.s_flag  = true;
          flags.s1_flag = true;
          flags.s2_flag = true;
          flags.s3_flag = true;
          flags.s4_flag = true;
          flags.s5_flag = true;

          check_s = true;
        }
        else if (e == 'T') /* FOR "LATEST", "DIRTIEST", "BOLDEST" */
          flags.t_flag = true;
        else if (e == 'V') /* FOR 'IVE */
          flags.v_flag = true;
        else if (e == 'X') /* FOR 'S */
          flags.x_flag = true;
        else if (e == 'Y') /* FOR "QUICKLY" */
          flags.y_flag = true;
        else if (e == 'Z') /* FOR 'R */
          flags.z_flag = true;
        else if (e == '#') /* FOR 'R */
          skip = true;
        else
          std::cerr << "Unhandled '" << e << "'\n";
      }
    }

    if (! skip)
      addBaseWord(word, flags);
  }

  for (auto &pw : wordFlags_) {
    const auto &word  = pw.first;
    auto       &flags = pw.second;

    auto len = word.size();
    if (len <= 1) continue;

    addDWord (word, flags);
    addEWord (word, flags);
    addGWord (word, flags);
    addHWord (word, flags);

    addRWord (word, flags);
    addTWord (word, flags);
    addYWord (word, flags);

    addN1Word(word, flags);
    addN2Word(word, flags);
    addN3Word(word, flags);

//  addIonsWord    (word, flags);
//  addIcationsWord(word, flags);
//  addEnsWord     (word, flags);
//  addIngsWord    (word, flags);
//  addErsWord     (word, flags);

//  addS1Word(word, flags);
//  addS2Word(word, flags);
//  addS3Word(word, flags);
    addS4Word(word, flags);
//  addS5Word(word, flags);
//  addS6Word(word, flags);
  }

  return true;
}

bool
CSpellTrie::
checkWord(const std::string &word) const
{
  auto pw = words_.find(word);

  return (pw != words_.end());
}

bool
CSpellTrie::
checkStartWord(const std::string &word) const
{
  return trie_.startWord(word);
}

void
CSpellTrie::
complete(const std::string &word, std::vector<std::string> &strs) const
{
  trie_.complete(word, strs);
}

// 'ed
void
CSpellTrie::
addDWord(const std::string &word, const WordFlags &flags)
{
  if (! flags.d_flag)
    return;

  auto word1 = word;

  auto len = word1.size();
  auto c   = word1[len - 1];

  if      (c == 'e')
    word1 = word1.substr(0, len - 1);
  else if (c == 'y' && ! isVowel(word1[len - 2]))
    word1 = word1.substr(0, len - 1) + "i";

  word1 += "ed";

  addExtraWord(word, word1);
}

// 'e, 'ive
void
CSpellTrie::
addEWord(const std::string &word, const WordFlags &flags)
{
  if (! flags.v_flag)
    return;

  auto word1 = word;

  auto len = word1.size();
  auto c   = word1[len - 1];

  if (c == 'e')
    word1 = word1.substr(0, len - 1);

  word1 += "ive";

  addExtraWord(word, word1);
}

// 'ing
void
CSpellTrie::
addGWord(const std::string &word, WordFlags &flags)
{
  if (! flags.g_flag)
    return;

  auto word1 = word;

  auto len = word1.size();
  auto c   = word1[len - 1];

  if (c == 'e')
    word1 = word1.substr(0, len - 1);

  word1 += "ing";

  flags.ing_flag = true;

  addExtraWord(word, word1);
}

// 'th
void
CSpellTrie::
addHWord(const std::string &word, const WordFlags &flags)
{
  if (! flags.h_flag)
    return;

  auto word1 = word;

//auto len = word1.size();
//auto c   = word1[len - 1];

  word1 += "th";

  addExtraWord(word, word1);
}

// 'en
void
CSpellTrie::
addN1Word(const std::string &word, WordFlags &flags)
{
  if (! flags.n1_flag)
    return;

  auto word1 = word;

  auto len = word1.size();
  auto c   = word1[len - 1];

  if (c == 'e' || c == 'y')
    return;

  word1 += "en";

  flags.en_flag = true;

  addExtraWord(word, word1);
}

// 'ion
void
CSpellTrie::
addN2Word(const std::string &word, WordFlags &flags)
{
  if (! flags.n2_flag)
    return;

  auto word1 = word;

  auto len = word1.size();
  auto c   = word1[len - 1];

  if (c == 'e')
    word1 = word1.substr(0, len - 1);

  word1 += "ion";

  flags.ion_flag = true;

  addExtraWord(word, word1);
}

// 'ication
void
CSpellTrie::
addN3Word(const std::string &word, WordFlags &flags)
{
  if (! flags.n3_flag)
    return;

  auto word1 = word;

  auto len = word1.size();
  auto c   = word1[len - 1];

  if      (c == 'e')
    word1 = word1.substr(0, len - 1);
  else if (c == 'y')
    word1 = word1.substr(0, len - 1);

  word1 += "ication";

  flags.ication_flag = true;

  addExtraWord(word, word1);
}

// 'er
void
CSpellTrie::
addRWord(const std::string &word, WordFlags &flags)
{
  if (! flags.r_flag)
    return;

  auto word1 = word;

  auto len = word1.size();
  auto c   = word1[len - 1];

  if      (c == 'e')
    word1 = word1.substr(0, len - 1);
  else if (c == 'y')  {
    auto c1 = (len > 2 ? word1[len - 2] : '\0');

    if (! isVowel(c1))
      word1 = word1.substr(0, len - 1) + "i";
  }

  word1 += "er";

  flags.er_flag = true;

  addExtraWord(word, word1);
}

// "IONS"
void
CSpellTrie::
addIonsWord(const std::string &word, const WordFlags &flags)
{
  if (! flags.s_flag)
    return;

  if (! flags.ion_flag)
    return;

  if (! flags.x_flag)
    return;

  auto word1 = word;

  auto len = word1.size();
  auto c   = word1[len - 1];

  if (c == 'e')
    word1 = word1.substr(0, len - 1);

  word1 += "ions";

  addExtraWord(word, word1);
}

// "ICATIONS"
void
CSpellTrie::
addIcationsWord(const std::string &word, const WordFlags &flags)
{
  if (! flags.s_flag)
    return;

  if (! flags.ication_flag)
    return;

  if (! flags.x_flag)
    return;

  auto word1 = word;

  word1 += "ications";

  addExtraWord(word, word1);
}

// "ENS"
void
CSpellTrie::
addEnsWord(const std::string &word, const WordFlags &flags)
{
  if (! flags.s_flag)
    return;

  if (! flags.en_flag)
    return;

  auto word1 = word;

  word1 += "ens";

  addExtraWord(word, word1);
}

// "INGS"
void
CSpellTrie::
addIngsWord(const std::string &word, const WordFlags &flags)
{
  if (! flags.s_flag)
    return;

  if (! flags.ing_flag)
    return;

  if (! flags.j_flag)
    return;

  auto word1 = word;

  auto len = word1.size();
  auto c   = word1[len - 1];

  if (c == 'e')
    word1 = word1.substr(0, len - 1);

  word1 += "ings";

  addExtraWord(word, word1);
}

// "ERS"
void
CSpellTrie::
addErsWord(const std::string &word, const WordFlags &flags)
{
  if (! flags.s_flag)
    return;

  if (! flags.er_flag)
    return;

  if (! flags.z_flag)
    return;

  auto word1 = word;

  auto len = word1.size();
  auto c   = word1[len - 1];

  if      (c == 'e')
    word1 = word1.substr(0, len - 1);
  else if (c == 'y')  {
    auto c1 = (len > 2 ? word1[len - 2] : '\0');

    if (! isVowel(c1))
      word1 = word1.substr(0, len - 1) + "i";
  }

  word1 += "ers";

  addExtraWord(word, word1);
}

// "IERS"
void
CSpellTrie::
addS1Word(const std::string &word, const WordFlags &flags)
{
  if (! flags.s_flag)
    return;

  if (! flags.z_flag)
    return;

  auto word1 = word;

  auto len = word.size();
  auto c   = word[len - 1];

  if (c == 'y' && ! isVowel(word1[len - 2]))
    word1 = word1.substr(0, len - 1);

  word1 += "iers";

  addExtraWord(word, word1);
}

// "IES"
void
CSpellTrie::
addS2Word(const std::string &word, const WordFlags &flags)
{
  if (! flags.s_flag)
    return;

  if (! flags.p_flag)
    return;

  auto len = word.size();
  auto c   = word[len - 1];

  if (flags.z_flag) {
    auto word1 = word;

    if (c == 'y' && ! isVowel(word1[len - 2]))
      word1 = word1.substr(0, len - 1);

    word1 += "ies";

    addExtraWord(word, word1);
  }
}

// "ES", "S"
void
CSpellTrie::
addS3Word(const std::string &word, const WordFlags &flags)
{
  if (! flags.s_flag)
    return;

  if (! flags.z_flag)
    return;

  auto word1 = word;

  word1 += "es";

  addExtraWord(word, word1);
}

void
CSpellTrie::
addS4Word(const std::string &word, const WordFlags &flags)
{
  if (! flags.s_flag)
    return;

  if (! flags.z_flag)
    return;

  auto len = word.size();
  auto c   = word[len - 1];

  auto word1 = word;

  if      (c == 'h')
    word1 = word1 + "e";
  else if (c == 'y' && ! isVowel(word1[len - 2]))
    word1 = word1.substr(0, len - 1) + "ie";
  else if (c == 's')
    word1 = word1 + "e";
  else if (c == 'x')
    word1 = word1 + "e";

  word1 += "s";

  addExtraWord(word, word1);
}

// "INESS", "NESS"
void
CSpellTrie::
addS5Word(const std::string &word, const WordFlags &flags)
{
  if (! flags.s_flag)
    return;

  auto len = word.size();

  auto c = word[len - 1];

  if (flags.p_flag) {
    auto word1 = word;

    if (c == 'y' && ! isVowel(word1[len - 2]))
      word1 = word1.substr(0, len - 1);

    word1 += "iness";

    addExtraWord(word, word1);
  }

  if (flags.p_flag) {
    auto word1 = word;

    word1 += "ness";

    addExtraWord(word, word1);
  }
}

#if 0
// "'S"
void
CSpellTrie::
addS6Word(const std::string &, const WordFlags &)
{
  if (! flags.s_flag)
    return;

  if (! flags.m_flag)
    return;

  if (flags.m_flag) {
    auto word1 = word;

    word1 += "'s";

    addExtraWord(word, word1);
  }
}
#endif

// 'est
void
CSpellTrie::
addTWord(const std::string &word, const WordFlags &flags)
{
  if (! flags.t_flag)
    return;

  auto word1 = word;

  auto len = word1.size();
  auto c   = word1[len - 1];

  if      (isVowel(c))
    word1 = word1.substr(0, len - 1);
  else if (c == 'y' && ! isVowel(word1[len - 2]))
    word1[len - 1] = 'i';

  word1 += "est";

  addExtraWord(word, word1);
}

// 'ly
void
CSpellTrie::
addYWord(const std::string &word, const WordFlags &flags)
{
  if (! flags.y_flag)
    return;

  auto word1 = word;

//auto len = word1.size();
//auto c   = word1[len - 1];

  word1 += "ly";

  addExtraWord(word, word1);
}

void
CSpellTrie::
addBaseWord(const std::string &word, const WordFlags &flags)
{
  auto pw = words_.find(word);
  assert(pw == words_.end());

  if (CSpellCheckWord(word) != 1) {
    std::cerr << "Warning: Bad Word: " << word << "\n";
  }

  trie_.addWord(word);

  wordFlags_[word] = flags;

  words_.insert(word);
}

void
CSpellTrie::
addExtraWord(const std::string &origWord, const std::string &word)
{
  auto pw = words_.find(word);

  if (pw != words_.end()) {
    //std::cerr << "Double Add Word: " << origWord << " -> " << word << "\n";
    return;
  }

  if (CSpellCheckWord(word) != 1) {
    std::cerr << "Warning: Bad Extra Word: " << origWord << " -> " << word << "\n";
  }

  trie_.addWord(word);

  words_.insert(word);
}

bool
CSpellTrie::
isVowel(char c) const
{
  switch (c) {
    case 'a': case 'e': case 'i': case 'o': case 'u': return true;
    default: return false;
  }
}
