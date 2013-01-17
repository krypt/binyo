Gem::Specification.new do |s|
  s.name = 'binyo'
  s.version = '0.0.1'
  s.author = 'Martin Bosslet'
  s.email = 'Martin.Bosslet@gmail.com'
  s.homepage = 'https://github.com/krypt/binyo'
  s.summary = 'Binary IO for C-based Rubies'
  s.extensions << 'ext/binyo/extconf.rb'
  s.files = %w(LICENSE) + Dir.glob('{bin,ext,lib,spec,test}/**/*')
  s.test_files = Dir.glob('test/**/test_*.rb')
  s.require_path = "lib"
end
