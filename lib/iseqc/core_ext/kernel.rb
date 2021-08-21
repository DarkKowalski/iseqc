# frozen_string_literal: true

module Kernel
  def iseq_packages
    @iseq_packages ||= {}
  end

  def require_iseq(path, from: nil)
    iseq_packages[from] ||= Iseqc::Unpack.unpack(from)
    begin
      iseq_packages[from].require(path)
    rescue StandardError
      raise LoadError, "Unable to load #{path} from #{from}"
    end
  end

  def load_iseq(path, from: nil)
    iseq_packages[from] ||= Iseqc::Unpack.unpack(from)
    begin
      iseq_packages[from].load(path)
    rescue StandardError
      raise LoadError, "Unable to load #{path} from #{from}"
    end
  end
end
