#include <CSpell.h>
#include <iostream>

int
main(int argc, char **argv)
{
  if (! CSpellInit())
    exit(1);

  for (int i = 1; i < argc; ++i) {
    int rc = CSpellCheckWord(argv[i]);

    if (rc == 0)
      std::cerr << "Mispelled: " << argv[i] << std::endl;
  }

  CSpellTerm();

  return 0;
}
