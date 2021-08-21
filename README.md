# iseqc
YARV Instruction Sequence compiler and linker for RubyGems

## Usage

Pack

```ruby
require 'iseqc'
package = 'example.rpk'
source_dir = 'example'
Iseqc::Pack.pack(package, source_dir)
```

Load

```ruby
require 'iseqc'
package = Iseqc::Unpack.unpack('example.rpk')
package.require 'file'
```

## Benchmark

```bash
$ ruby bench/gen_cases.rb
$ ruby bench/bench.rb
```

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
