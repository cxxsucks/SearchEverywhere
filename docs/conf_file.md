# Configuration Files

TODO

## `orient` Configuration File

TODO

### Root Paths

Root paths are where filesystem indexing start. For Windows users, these
are typically drive paths like `C:\ D:\`.  
The stock settings may work out of the box, but take note of the `HDD` and
`SSD` root paths. All root paths must correspond to the type of disk it
is on, otherwise indexing speed will suffer a great drop.  
If there are any HDD root paths not listed, be sure to add them.  
When unsure, place all root paths into SSD one, since the majority of modern
computers are running on random-access-friendly SSDs.

Root paths CAN overlap with, but CANNOT be identical to, other root paths
or ignored paths. Here, *overlapping* means a directory being the direct
or indirect child of the other directory, like `/home/user/Documents`
overlaps with `/home` since `/home` contains `/home/user/Documents`.  

When root paths overlap, the "deepest" directory is first scanned and
converted into index, which is later reused by its parent. Adding overlapping
root paths could reduce peak memory usage during indexing process, and
that is why on Windows `Program Files` directory in each drive is also
listed as a root path.  

TODO: Root paths identical causes rescanning.

### Ignored Paths

All ignored paths are, well, ignored while indexing.  
Typically infrequently used or volatile paths are ignored, like external
drives, Linux auto filesystems and temporary files. Default setting include
all of them (aside from `/dev /sys` on Linux).

**Windows Users Note**: By default `C:\Windows` is ignored as it is scarsely
accessed by normal users and has *many, many* files, about 1 million or so.
Remove it from ignored paths if you insist on indexing it. According to
previous section, `C:\Windows` should become a root path if it is indexed.
**Also Note**: Some Windows systems' `C:\Windows` is called `C:\WINDOWS`.
`SearchEverywhere` is case sensitive, therefore double check what your
`C:\Windows` is named if indexing is taking too long.

TODO: Ignored paths overlapping & identical

## `SearchEverywhere` Configuration File

TODO
