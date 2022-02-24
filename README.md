# wayclip

wayclip is a pair of command-line utilities: `waycopy` and `waypaste`,
which allow access to the Wayland clipboard. Specifically, `wayclip`
is a `wlr-data-control` protocol client.

wayclip distinguishes itself from other Wayland clipboard utilities
in the following ways:
- small - the whole codebase is less than 300 lines! (excluding whitespace)
- Unixy - all it does is copy and paste; other functions are for other programs
- no dynamic memory allocation

## Usage

To copy data, just pipe it into `waycopy`, and optionally specify a seat and mimetype:

```
$ echo "howdy" | waycopy -s seat0 -t text/plain &
```
(note that `waycopy` does not automatically fork into the background)

To paste, optionally specify a seat and desired mimetype:
```
$ waypaste -s seat0 -t text/plain
howdy
$
```
