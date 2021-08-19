module Kernel
  def iseq_packages
    @iseq_packages ||= { }
  end

  def require_iseq(path, from: nil)
    iseq_packages[from] ||= Iseqc::Unpack.unpack(from)
    iseq_packages[from].require(path)
  end

  def load_iseq(path, from: nil)
    iseq_packages[from] ||= Iseqc::Unpack.unpack(from)
    iseq_packages[from].load(path)
  end
end
