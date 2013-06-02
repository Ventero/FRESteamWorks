#!/usr/bin/ruby

contents = open("API.txt", "r").read.split("\n")

linuxLib = !ARGV[0].nil?

defaults = Hash.new("null");
defaults["Boolean"] = "false"
defaults["Number"] = "0.0"
defaults["int"] = "0"
defaults["uint"] = "0"
defaults["String"] = "\"\""

types = Hash.new { |h,k| "readResponse() as #{k}"}
types["Boolean"] = "readBoolResponse()"
types["int"] = "readIntResponse()"
types["uint"] = "readIntResponse()"
types["Number"] = "readFloatResponse()"
types["String"] = "readStringResponse()"

num = 0
# skip first line (init) when generating actual functions, since it has a
# special implementation in both libs
contents[ARGV[0] == "f" ? 0 : 1,contents.size].each do |line|
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
		puts <<EOD
		#{line} {
			if(!callWrapper(#{func_name}, [#{arg_names.join(", ")}])) return #{defaults[ret]};
			return #{types[ret]};
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
