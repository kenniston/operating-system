# **KaShell**

***KaShell*** is a miniature shell implementation. This shell can create tasks 
that run programs in Linux Jobs and Process. Programs can be executed through pipes, 
which allows communication between processes on the pipiline. 

*Updated: 3 Sep 2021*

<br/>

## How to run the project

You can compile and run this project using CMake. Follow the steps below to install the required tools:

```sh
$ sudo apt update
```
```sh
$ sudo apt install build-essential
```

---

```sh
$ wget https://github.com/Kitware/CMake/releases/download/v3.21.2/cmake-3.21.2.tar.gz
```

```sh
$ tar -xvf cmake-3.21.2.tar.gz
```

---

```sh
$ cd cmake-3.21.2
```
```sh
$ ./bootstrap
```
```sh
$ make
```
```sh
$ sudo make install
```

---

Build KaShell:

```sh
$ cmake -S . -B release
```
```sh
$ cmake --build release
```

Run KaShell:
```sh
$ release/kashell
```

To install KaShell and the KaShell Man Page:
```sh
$ sudo cmake --install release
```

<br/>
