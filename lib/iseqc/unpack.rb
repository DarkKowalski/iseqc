# frozen_string_literal: true

module Iseqc
  module Unpack
    def self.unpack(pkg)
      Package.new(pkg)
    end

    class Package
      attr_reader :pkg, :mri_version, :page_size, :index_size, :iseq_start, :index, :memory_map

      def initialize(pkg)
        meta = Iseqc::Unpack.load_metadata(pkg)
        @pkg = pkg
        @mri_version = meta[0]
        @page_size = meta[1]
        @index_size = meta[2]
        @iseq_start = meta[3]
        @index = Iseqc::Index.load(meta[4])
        @memory_map = {}
      end

      def load(entry)
        internal_path = Iseqc::Index.internal_path(entry)
        load_into_memory(internal_path)
        eval(internal_path)
      end

      def require(entry)
        internal_path = Iseqc::Index.internal_path(entry)
        return false if $LOADED_FEATURES.include?(feature(internal_path))

        load_into_memory(internal_path)
        eval(internal_path)
        add_feature(internal_path)

        true
      end

      private

      def load_into_memory(internal_path)
        return if @memory_map.key?(internal_path)

        target = @index.lookup(internal_path)
        iseq = Iseqc::Unpack.load_iseq(pkg, @iseq_start, target[:offset], target[:size])
        @memory_map[internal_path] = iseq
      end

      def eval(internal_path)
        iseq_bin = @memory_map[internal_path]
        RubyVM::InstructionSequence.load_from_binary(iseq_bin).eval
      end

      def add_feature(entry)
        $LOADED_FEATURES << feature(entry)
      end

      def feature(internal_path)
        iseq_bin = @memory_map[internal_path]
        "#{pkg}:#{internal_path}"
      end
    end
  end
end
