# treedude

treedude is a clone of [the mini-game from Superhot titled 'Tree Dude Tree Dude Dude'](https://superhot.fandom.com/wiki/Tree_Dude). Chop as much of the tree as you can while avoiding branches and keeping an eye on the timer.

Written in ANSI C with the Curses library, complies with [SUSv2](https://pubs.opengroup.org/onlinepubs/007908799/).

## Installation

Ensure the following is available on your system:
* GCC or clang. To use clang, `CC=clang` must be passed to make commands.
* An [X/Open Curses](https://pubs.opengroup.org/onlinepubs/7908799/xcurses/curses.h.html) implementation such as ncurses or PDCurses.

Clone and build:
(Replace `github.com` with `gitlab.com` if using GitLab)
```
$ git clone https://github.com/n-ivkovic/treedude
$ cd treedude
$ make
```

Run without installing:
```
$ ./treedude
```

Install and run:
```
# make install
$ treedude
```

Update after installation:
```
$ git pull master
$ make
# make install
```

Additional options:
```
$ make help
```

## Usage

### Controls

| Key           | Action     |
| ---           | ---        |
| `←`, `A`, `H` | Chop left  |
| `→`, `D`, `L` | Chop right |
| `Enter`       | Continue   |
| `Q`, `Esc`    | Quit       |

### Options

| Option | Description |
| ---    | ---         |
| -n     | Do not read or write the high score from the `$XDG_DATA_HOME` (`$HOME`/.local/share) directory. |
| -v, -V | Print version and exit. |

## Wishlist

The below are features I may or may not get around to implementing:

* Windows support. This will need to include changing the build process, likely through the usage of CMake.
* Refactors and rewrites. This is my first notable C project, there are bound to be areas that need improvement.

## Contributing

Please adhere to the following when creating a pull request:

* Ensure changes do not return any warnings during compile time.
* Ensure changes are written in ANSI C that complies with [SUSv2](https://pubs.opengroup.org/onlinepubs/007908799/).
* Ensure changes match the coding style of the project. No specific style is adhered to, but I suppose the [Linux kernel coding style](https://www.kernel.org/doc/html/v4.10/process/coding-style.html) is a good one.
* Ensure the [Git Flow](https://nvie.com/posts/a-successful-git-branching-model/) branching model is (somewhat) adhered to. Ensure changes are branched from `develop` and the pull request merges back into `develop`. Note that before the PR is accepted the target branch may be changed to a new branch named either `feature/[branch-name]` or `fix/[branch-name]`.

## Licence

Copyright © 2022 Nicholas Ivkovic.

Licensed under the GNU General Public License version 3 or later. See [./LICENSE](./LICENSE), or [https://gnu.org/licenses/gpl.html](https://gnu.org/licenses/gpl.html) if more recent, for details.

This is free software: you are free to change and redistribute it. There is NO WARRANTY, to the extent permitted by law.
