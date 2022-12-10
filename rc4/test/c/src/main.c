#include <string.h>
#include "simple_test.h"
#include "random_test.h"

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		int do_random_test = 0;
		const char *seed = NULL;
		for (int i = 1; i < argc; i++)
		{
			if (strcmp(argv[i], "-r") == 0)
			{
				do_random_test = 1;
				if (i + 1 < argc)
					seed = argv[i + 1];
			}
		}

		if (do_random_test)
		{
			random_test(seed);
			return 0;
		}
	}

	simple_test();
}