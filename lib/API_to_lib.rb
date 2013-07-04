#!/usr/bin/ruby

contents = open("API.txt", "r").read.split("\n")

linuxLib = !ARGV[0].nil?

defaults = Hash.new { |h,k| ["readResponse() as #{k}", "null"] }
defaults["Boolean"] = ["readBoolResponse()", "false"]
defaults["Number"] = ["readIntResponse()", "0.0"]
defaults["int"] = ["readIntResponse()", "0"]
defaults["uint"] = ["readFloatResponse()", "0"]
defaults["String"] = ["readStringResponse()", "\"\""]

num = 0
# skip first line (init) when generating actual functions, since it has a
# special implementation in both libs
contents.drop(1).each do |line|
	if line.empty? or line[0].chr == "/"
		puts "\t\t#{line}".rstrip
		next
	end

	func = line.split("(")[0].split[-1]

	ret = line.split(":")[-1]
	args = line[/\(([^)]*)\)/, 1]
	arg_names = args.split(/,\s*/).map do |arg|
		arg.split(":")[0]
	end
	func_name = "AIRSteam_#{func[0].chr.upcase + func[1,func.size]}"

	case ARGV[0]
	when "f" then
		puts "\t\tprivate static const #{func_name}:int = #{num};"
	when "l" then
		type, default = defaults[ret]
		puts <<EOD
		#{line} {
			if(!callWrapper(#{func_name}, [#{arg_names.join(", ")}])) return #{default};
			return #{type};
		}

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
