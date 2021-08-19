# iseqc
YARV Instruction Sequence compiler and linker for RubyGems

## Format

### File

```
+----------------------------------------+
|                  File                  |
+--------+-------+--------+-----+--------+
| Header | Index | Iseq 0 | ... | Iseq x |
+--------+-------+--------+-----+--------+
```

### Header

```
+---------------------------------------------+
|              Header: 256 bits               |
+-----------------------+---------------------+
| MRI Version:  32 bits | Page Size:  32 bits |
+-----------------------+---------------------+
| Index Size:   32 bits | Iseq Start: 32 bits |
+-----------------------+---------------------+
| File CRC32:   32 bits | Padding:    32 bits |
+-----------------------+---------------------+
|               Padding: 32 bits              |
+-----------------------+---------------------+
```

### Index

```ruby
# Marshal dump/load Hash
{
  '/usr/bin/ruby' => { offset: 0, size: 114514 },
  '/home/kowalski/example.txt' => { offset: 114514, size: 1919 }
}
```
