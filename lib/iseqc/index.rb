# frozen_string_literal: true

module Iseqc
  class Index
    attr_reader :index

    def initialize(src = nil, ext: '.rbc', index: nil)
      unless index.nil?
        @index = index
        return
      end
      @index = scan(src, ext)
    end

    def self.load(bin)
      Index.new(index: Marshal.load(bin))
    end

    # FIXME: Can be improved!
    def self.internal_path(path)
      # Pathname.new(File::SEPARATOR + path).cleanpath.to_s.freeze
      File::SEPARATOR + path
    end

    def dump
      Marshal.dump(@index)
    end

    def lookup(internal_path)
      @index[internal_path]
    end

    private

    def scan(src, ext)
      prefix = File.realpath(src)
      pagesize = Iseqc::System.pagesize
      offset = 0

      result = {}

      Dir[File.join(src, '**', "*#{ext}")].map do |f|
        size = File.size(f)
        path = File.realpath(f).delete_prefix(prefix)

        # Align memory for mmap
        unaligned = offset % pagesize
        padding = unaligned.zero? ? 0 : pagesize - unaligned
        offset += padding

        result[path] ||= { offset: offset, size: size }
        offset += size
      end

      result
    end
  end
end
