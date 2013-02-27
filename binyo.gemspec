Gem::Specification.new do |s|

  s.name = 'binyo'
  s.version = '0.0.1'

  s.author = 'Martin Bosslet'
  s.email = 'Martin.Bosslet@gmail.com'
  s.homepage = 'https://github.com/krypt/binyo'
  s.summary = 'Fast binary IO for Ruby'
  s.description = 'binyo offers a generic C API for dealing with Ruby IO objects and extension classes that allow to deal effectively with binary data'

  s.required_ruby_version     = '>= 1.9.3'

  s.extensions << 'ext/binyo/extconf.rb'
  s.files = %w(LICENSE) + Dir.glob('{bin,ext,lib,spec,test}/**/*')
  s.test_files = Dir.glob('test/**/test_*.rb')
  s.require_path = "lib"
  s.license = 'MIT'

end
