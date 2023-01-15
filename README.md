# SearchEverywhere

First Fully-Libre GUI File Indexer on macOS and Windows:  
![stolenLogo](assets/search_img/stolenLogo.ico)
**Search***Everywhere*  
^ The icon is stolen from [a repo](https://github.com/Jycjmf/SearchEverywhere)
whose name clashed with mine LOL.

(brief introduction)

## **Important** Caveat on Linux

For some reason, which seems like a kernel or Qt issue, memory usage on
Linux grows rather quickly. On my laptop with 1.2M files, it starts up
at 65MiB, and several searches or updatedbs later, memory usage skyrockets
before ultimately sitting between 350~400MiB.
(Less than Chromium/Electron😅)

Only on Linux do this happen. On both Windows and macOS, memory usage sit
well below 120MiB when idle, with short spikes to \<200MiB during updatedb,
after which memory usage falls back.

`valgrind` detected no leaking issues either, suggesting it may probably a
framework or system issue. Feel free to talk on issue tracker and discussion
panel here.  
For now however, a good solution is to *restart the app often* as if it
were a memory leaking monster, or better, use linux-y `orient` CLI.

## Credits

### The Name and Icon

While aimlessly browsing DuckDuckGo and GitHub for an icon of *this*
`SearchEverywhere`, I came across another
[SearchEverywhere](https://github.com/Jycjmf/SearchEverywhere).  
It seems to be Windows only and dependent on non-*free-as-in-freedom*
`Everything`, which kinda ruins the purpose of it using *free-as-in-freedom*
GPLv3 license. Despite lacking core functionalities, its UI is far beyond
this `SearchEverywhere`'s reach and, above all, contains an icon that
literally means `SearchEverywhere` (because that's also its name).

And, per Stallman's wish, I declared the icon independent of its original
non-free dependency `Everything`, aka, stole it.  

Do [check that SearchEverywhere out](https://github.com/Jycjmf/SearchEverywhere),
even though it is in Chinese and **MSWindows Only😭**.

### Others

- Qt for GUI Framework
- LibreOffice for Office document previewing
