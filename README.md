# EggPM

EggPM is a package manager that isn't that good so it's best not to use it.

For now, EggPM installs packages into `/tmp/eggpmtest` because I don't trust 
my computer with it just yet. This will be changed in a future update.

Examples for config files and repositories can be found in /examples

## Index

- [FAQ](#faq)
- [Usage](#usage)
- [Config](#config)
- [Installation](#installation)
- [Repositories](#repositories)
- [Build Process](#build-process)
- [File Structure](#file-structure)
- [Bug Reports / Questions](#bug-reports--questions)

## FAQ

### Q: Why did you make this?

A: I was bored.

### Q: What are your plans for this?

A: Just slowly improve on it and eventually it might be somewhat decent.

### Q: What will you do about your repository going offline?

A: I've left copies of files in /examples, but as the amount of packages
increases, I might make a seperate git repository with all `build.sh`s and 
some basic packages.

### Q: Should I ditch apt, yum, pacman, or others for this?

A: Please don't.

### Q: My computer is on fire, can I use `eggpm -i computer`?

A: Sadly no.

### Q: How do i verify this code integrity with PGP?

A: [https://xkcd.com/1181/](https://xkcd.com/1181/)

## Usage

EggPM only has a few commands at this stage.

### Updating a repository:

`eggpm -S`

### Installing a package:

`eggpm -i [package]`

### Building a package from source

[dir] is the directory with a build.sh file:

`eggpm -b [dir]`

## Config

The default repository is `https://baguye.uk/eggpm/repo`

The config file `eggpm.conf` is stored in sysconfdir, aka PREFIX/etc

EggPM does not install a default config file, although it should now that I
think about it...

For now, the config only has one option, repository.

The syntax is simply key=value

To add a repository to EggPM, just add the following to eggpm.conf

`repository=[URL]`

There is an example in /examples so this can't be messed up.

## Installation

EggPM uses autotools for its build process. 
The following will usually be all that's needed.

```sh
./configure
make
make install
```

Dependencies are shown below, although versions aren't specific because I don't 
know, just aim for the newest available at the time of this release.

```
libarchive >= 3.0.0
libxml2 >= 2.0.0
openssl >= 3.0.0
sqlite3 >= 3.0.0
libcurl >= 8.0.0
```

## Repositories

To make a repository for EggPM, it's an xml with this structure.

```xml
<repo>
    <package name="example">
        <!--package info here-->
    </package>
    <package name="another_example">
        <!--package info here-->
    </package>
</repo>
```

Eventually there will be support for JSON repositories too, but we'll see.

To get the package info, just use the output from `eggpm -b` from earlier,
or use the example in /examples and just change the figures. Make sure to 
change the `url` section to where the file is located.

## Build process

EggPM builds packages based on a `build.sh` file.

Variables like name, version are defined as normal bash variables.

The variables `stage00` - `stage99` are used for stages in the build process.

Here is the file tree before building. For this section, it'll show the process
of building the GNU package `hello-2.12.1`.

```
hello
└── build.sh
```

First, EggPM runs the script with `bash` and extracts variables afterwards.
Then, it downloads the file in `url` and matches it with the sha256 checksum in
`checksum`. It extracts the file with `tar -xf`. Finally, it enters the main
directory, so if you run `eggpm -b packages/hello`, it would put you in 
`packages/hello`.

```
hello
├── build.sh
├── hello-2.12.1
│   ├── ...
└── hello-2.12.1.tar.gz
```

Now is when the real build starts. It goes through all the stages in order.
Due to... issues, `cd` command have to be in their own stage, otherwise they
won't apply for any other stage. For most packages, the first stage would be
`cd name-version`. A typical package will just use `./configure` and `make` and
`make install` to build it. Note that packages have to be compiled as if
they're in `/usr`, and installed into the `build` directory.

Here is an example simplified directory after building `hello-2.12.1`

```
hello
├── build
│   ├── usr
│   │   ├── bin
│   │   │   ├── hello
├── build.sh
├── hello-2.12.1
│   ├── ...
└── hello-2.12.1.tar.gz
```

After the stages, EggPM returns to its own code, and packages the contents of
`build` into a `.eggpm` file. Afterwards, it removes the `build` directory and
the original file that it originally downloaded. The final `.eggpm` is put in
`dist`, and there is also a message by EggPM telling you the exact path.

```
hello
├── dist
│   ├── hello-2.12.1.eggpm
└── build.sh
```

Finally EggPM prints lots of info about how to add this package to a 
repository, so it's mostly copying the XML it gives.

## File structure

This is too in-depth for the README, but it might be useful to someone.

The `.eggpm` file format is just a `.tar.xz` archive with a different
extension. The files in it are from the root directory, so for example,
`/usr/bin/hello` would be put in `./usr/bin/hello` in the file. Eventually,
there might be a few scripts in the file for what to do post installation but
that is for another time. 

## Bug Reports / Questions

If you're using this program for some reason and you find a bug, either use the
[Github issue tracker](https://github.com/BaguetteYeeter/eggpm/issues) or send an email to [baguetteyeeter@icloud.com](mailto:baguetteyeeter@icloud.com)