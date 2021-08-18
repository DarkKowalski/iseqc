# frozen_string_literal: true

module Iseqc
  class Index
    attr_reader :index

    def initialize(src, ext: '.rbc')
      @index = scan(src, ext)
    end

    def self.load(bin)
      Marshal.load(bin)
    end

    def dump
      Marshal.dump(@index)
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
