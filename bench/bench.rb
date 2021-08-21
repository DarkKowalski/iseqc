# frozen_string_literal: true

require_relative '../lib/iseqc'
require 'benchmark'

cases = File.join('.', File.dirname(__FILE__), '..', 'tmp', 'bench', 'cases')
package = 'bench.rpk'
raw = Dir[File.join(cases, '*.rb')]
packed = raw.map { |src| "#{File.basename(src)}.rbc" }
iterations = 100

Iseqc::Pack.pack(package, cases)

Benchmark.bmbm do |x|
  x.report('Kernel#require') do
    iterations.times do
      raw.each { |r| require r }
    end
  end

  x.report('Iseqc#require_iseq') do
    pkg = Iseqc::Unpack.unpack(package)
    iterations.times do
      packed.each { |rbc| pkg.require rbc }
    end
  end

  x.report('Kernel#load') do
    iterations.times do
      raw.each { |r| load r }
    end
  end

  x.report('Iseqc#load_iseq') do
    pkg = Iseqc::Unpack.unpack(package)
    iterations.times do
      packed.each { |rbc| pkg.load rbc }
    end
  end

end
