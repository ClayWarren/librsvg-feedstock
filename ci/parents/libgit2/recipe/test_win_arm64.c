#include <stdio.h>

#include <git2.h>

int main(void)
{
    int major = 0;
    int minor = 0;
    int revision = 0;

    if (git_libgit2_init() < 1)
    {
        return 1;
    }

    git_libgit2_version(&major, &minor, &revision);
    printf("libgit2 %d.%d.%d\n", major, minor, revision);

    if (major != LIBGIT2_VER_MAJOR || minor != LIBGIT2_VER_MINOR)
    {
        git_libgit2_shutdown();
        return 1;
    }

    return git_libgit2_shutdown() == 0 ? 0 : 1;
}
