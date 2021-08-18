# frozen_string_literal: true

module Iseqc
  module Iseq
    def self.compile_file(path, internal_path)
      options = {
        inline_const_cache: true,
        instructions_unification: true,
        operands_unification: true,
        peephole_optimization: true,
        specialized_instruction: true,
        stack_caching: true,
        tailcall_optimization: true,
        trace_instruction: false
      }.freeze

      bin = RubyVM::InstructionSequence.compile(File.read(path), internal_path, internal_path,
                                                options: options).to_binary
      File.write("#{path}.rbc", bin)

      bin
    end

    def self.compile_all(dir, ext: '.rb')
      # Resolve internal paths
      prefix = File.realpath(dir)
      args = Dir[File.join(dir, '**', "*#{ext}")].map do |src|
        internal_path = File.realpath(src).delete_prefix(prefix)
        [src, internal_path]
      end

      args.each { |arg| compile_file(*arg) }
    end
  end
end
