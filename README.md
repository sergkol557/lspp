# `ls++`
> An alternative to the `ls` *command* with display of **icons** of files and folders and with **colors**.

![ls++ running in current directory/folder](assets/lspp.png) 

---

## Dependencies
+ Install [Fonts](https://github.com/terroo/fonts)

---

## Build and install
```bash
cmake -B build .
cmake --build build
sudo cmake --install build
```

---

## Usage
+ Running in Current Directory/Folder
+ Running on Directory/Folder as argument and/or with multiple
+ Does not ignore entries starting with '.'
+ Use a long listing format
+ Both: all with list

![Examples 01](assets/args.png) 

![Examples 02](assets/all.png) 

Help:
```bash
lss++ --help
```

Set as your default `ls`:
```bash
echo 'alias ls="/usr/local/bin/ls++"' >> ~/.bashrc
source ~/.bashrc
```


