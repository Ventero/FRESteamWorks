#!/usr/bin/ruby

contents = open("API.txt", "r").read.split("\n")

linuxLib = !ARGV[0].nil?

defaults = Hash.new { |h,k| ["readResponse() as #{k}", "null"] }
defaults["Boolean"] = ["readBoolResponse()", "false"]
defaults["Number"] = ["readFloatResponse()", "0.0"]
defaults["int"] = ["readIntResponse()", "0"]
defaults["uint"] = ["readIntResponse()", "0"]
defaults["String"] = ["readStringResponse()", "\"\""]

num = 0
# skip first line (init) when generating actual functions, since it has a
# special implementation in both libs
skip = (ARGV[0] == "f" or ARGV[0] == "c") ? 0 : 1
contents.drop(skip).each do |line|
	if line.empty? or line[0].chr == "/"
		puts "\t\t#{line}".rstrip
		next
	end

	match = line.match /function ([^(]+)\(([^)]*)\):(.+)/
	($stderr.puts "Invalid line: #{line}"; next) unless match

	func, args, ret = match.captures
	arg_names = args.split(/,\s*/).map do |arg|
		arg.split(":")[0]
	end
	func_name = "AIRSteam_#{func[0].chr.upcase + func[1,func.size]}"

	case ARGV[0]
	when "f" then
		puts <<EOD
		private static const #{func_name}:int = #{num};
EOD
	when "l" then
		type, default = defaults[ret]
		puts <<EOD
		#{line} {
			if(!callWrapper(#{func_name}, [#{arg_names.join(", ")}])) return #{default};
			return #{type};
		}

EOD
	when "c" then
		puts <<EOD
		X(#{func_name}) /* = #{num} */
EOD
	else
		puts <<EOD
		#{line}
		{
			return _ExtensionContext.call(#{["func_name", *arg_names].join(", ")}) as #{ret};
		}

EOD
	end

	num += 1
end
