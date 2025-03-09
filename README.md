> [!IMPORTANT]
> This code is not actively maintained. Do not expect updates to this code (especially since the current Android backup format is at the end of its life). Feel free to open issues, but do not expect a quick reply (or even a reply at all). Patches are welcome (to things outside of the `shared` folder), but do not expect quick review and merge.


A simple GUI viewer for Signal Android backup files. I wrote this a long time ago, when backup files first appeared for Signal. When more and more often people needed help modifying their backups (mostly fixing them), I added backup-writing functions to the SignalBackup-class and released [signalbackup-tools](https://github.com/bepaald/signalbackup-tools). That program has now been around for years, has grown immensely and is actively maintained. This program is _not_ actively maintained.

The code dumped here is provided as-is. It was quickly updated to support current backups, and redo the emoji-stuff. Other than that it is pretty much untouched as I left it 6 years ago. I was (and still am) unfamiliar with QT at the time, and was learning about SQL as I went along. I'd say the code quality is not great. Especially the paint() and sizeHint() methods are very complicated and could use thorough refactoring.

#Compiling

First, install dependencies ('devel' packages if your distribution provides them separately):
 - Qt6 (QtWidget, QtSql)
 - openssl
 - vlc

After that, the following two commands build the program on my machine without warnings:

```Shell
$ qmake6
$ make -j
```

I can imagine the `qmake6` command might exist under different names on other systems.

#Running

```Shell
$ signalbackup-viewer [backup-file] [passphrase]
```
The look is based on the way Signal looked back then. This means speech bubbles with a little point on them, and the color on the side of the chat partner. Ctrl-F should open a search bar at the bottom. Double clicking on a message shows some info in the terminal (which was originally meant to open a sidebar with that info + save-options for media).



There is a long list of features this viewer does not yet support. Many of them simply did not exist when I last touched this code.
- I was apparently midway implementing quotes, when I stopped working on this: they half-appear.
- chat folders, emoji reactions, message-editing, message-styling, mentions, stories: none of these existed back then and none are supported.
- There is currently no way of playing video-media (audio should play back), or enlarging images. Also no saving yet.
- Many status messages are not implemented.
- Expect a _lot_ of debugging messages output to terminal while running.
- many, _many_ more....

#Future

In the short term, nothing much will happen here, although I will probably implement a few simple features (finishing quotes and implementing mentions and text styling might not be too difficult) when I'm bored and have nothing else to do.

In the long term, maybe, when the new Signal backup format has appeared and I know it well enough, I might start actively updating this program. This could be many years from now.
