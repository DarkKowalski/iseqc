# frozen_string_literal: true

require 'fileutils'

def gen_large_def(line: 10_100)
  large_def = "def large\n"
  line.times { |i| large_def += "  puts #{i}\n" }
  large_def += "end\n"

  large_def
end

def gen_large_literal(line: 1000)
  large_literal = "<<~TEXT\n"
  line.times { |_i| large_literal += "#{'a' * 1000}\n" }
  large_literal += "TEXT\n"

  large_literal
end

def gen_large_class(line: 10_000)
  large_class = "class Large\n"
  large_class += "def large\n"
  line.times { |i| large_class += "  puts #{i}\n" }
  large_class += "end\n" * 2

  large_class
end

def gen(dup: 10)
  cases = File.expand_path(File.join(File.dirname(__FILE__), '..', 'tmp', 'bench', 'cases'))
  FileUtils.mkdir_p cases

  case_large_def = File.join(cases, 'large_def.rb')
  File.write(case_large_def, gen_large_def * dup)

  case_large_class = File.join(cases, 'large_class.rb')
  File.write(case_large_class, gen_large_class * dup)

  case_large_literal = File.join(cases, 'large_literal.rb')
  File.write(case_large_literal, gen_large_literal * dup)
end

gen
