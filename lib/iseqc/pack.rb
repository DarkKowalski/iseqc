# frozen_string_literal: true

module Iseqc
  module Pack
    def self.pack(pkg, src, src_ext: '.rb', iseq_ext: '.rbc')
      Iseqc::Iseq.compile_all(pkg, src, ext: src_ext)
      index = Iseqc::Index.new(src)

      new_package(pkg)
      add_index(pkg, index.dump)

      index.index.each do |k, v|
        iseq = File.read(File.join(src, k))
        append_iseq(pkg, iseq, v[:offset])
      end
    end
  end
end
