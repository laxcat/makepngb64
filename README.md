# MakePNG(Base64)

### Build:
```
clang++ main.cpp -o makepngb64
```

### Usage:
```
> makepngb64 2x2x2 0xaabb                                             
2x2 PNG, filled with R:0xaa, G:0xbb for every 2-byte pixel:
iVBORw0KGgoAAAANSUhEUgAAAAIAAAACCAQAAADYv8WvAAAAEklEQVR4XmNctZuBgYmBgYEBAAvwAWmPyz9MAAAAAElFTkSuQmCC

> makepngb64 16x16x4 0xffffffff 
16x16 PNG, filled with R:0xff, G:0xff, B:0xff, A:0xff for every 4-byte pixel:
iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAIUlEQVR4XmP8DwQMFAAmBgrBqAEMDKNhMBoGoGw08OkAAGvKBBxbr9PgAAAAAElFTkSuQmCC
```
