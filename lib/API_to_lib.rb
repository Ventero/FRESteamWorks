#!/usr/bin/ruby

mode = ARGV[0]
if not ARGV[0] or not "fcliw".include? mode then
	$stderr.puts <<-EOD
Usage:
	ruby #$0 [f|c|l|w]
		f: AS3 function enum values
		c: C function macros
		l: Linux AS3 lib implementations
		w: Windows/OS X AS3 lib implementations
	EOD
	exit
end

contents = open("API.txt", "r").read.split("\n")

# maps function return types to the corresponding utility functions and
# default values
defaults = Hash.new { |h,k| ["readResponse() as #{k}", "null"] }
defaults["Boolean"] = ["readBoolResponse()", "false"]
defaults["Number"] = ["readFloatResponse()", "0.0"]
defaults["int"] = ["readIntResponse()", "0"]
defaults["uint"] = ["readIntResponse()", "0"]
defaults["String"] = ["readStringResponse()", "\"\""]

# functions to skip when generating code
ignore = Hash.new []
ignore["l"] = ["init", "runCallbacks", "useCrashHandler", "fileRead", "UGCRead"]
ignore["w"] = ["init"]

num = 0
contents.each do |line|
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

	next if ignore[mode].include? func

	case mode
	when "f" then
		puts <<-EOD
		private static const #{func_name}:int = #{num};
		EOD
	when "l" then
		type, default = defaults[ret]
		puts <<-EOD
		#{line} {
			if(!callWrapper(#{func_name}, [#{arg_names.join(", ")}])) return #{default};
			return #{type};
		}

		EOD
	when "c" then
		puts <<-EOD
		X(#{func_name}) /* = #{num} */
		EOD
	when "i" then
		puts <<-EOD
		#{line.sub("public ", "")}
		EOD
	else
		puts <<-EOD
		#{line}
		{
			return _ExtensionContext.call("#{func_name}"#{["", *arg_names].join(", ")}) as #{ret};
		}

		EOD
	end

	num += 1
end
