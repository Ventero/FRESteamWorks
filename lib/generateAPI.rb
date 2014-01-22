#!/usr/bin/ruby

def create_interface line, func, num, args, ret
	line.sub("public ", "")
end

def create_lib line, func, num, args, ret
	<<-EOD
#{line}
{
	return _ExtensionContext.call("#{func}"#{["", *args].join(", ")}) as #{ret};
}
	EOD
end

@defaults = Hash.new { |h,k| ["readResponse() as #{k}", "null"] }
@defaults["Boolean"] = ["readBoolResponse()", "false"]
@defaults["Number"] = ["readFloatResponse()", "0.0"]
@defaults["int"] = ["readIntResponse()", "0"]
@defaults["uint"] = ["readIntResponse()", "0"]
@defaults["String"] = ["readStringResponse()", "\"\""]

def create_lib_linux line, func, num, args, ret
	type, default = @defaults[ret]

	<<-EOD
#{line} {
	if(!callWrapper(#{func}, [#{args.join(", ")}])) return #{default};
	return #{type};
}
	EOD
end

def create_constants_linux line, func, num, args, ret
	"private static const #{func}:int = #{num};"
end

def create_macro line, func, num, args, ret
	"X(#{func}) /* = #{num} */"
end


files = [
	{
		:file => "src/com/amanitadesign/steam/ISteamWorks.as",
		:ignore => ["init"],
		:format => method(:create_interface)
	},
	{
		:file => "src/com/amanitadesign/steam/FRESteamWorks.as",
		:ignore => ["init"],
		:format => method(:create_lib)
	},
	{
		:file => "src_linux/com/amanitadesign/steam/FRESteamWorks.as",
		:ignore => ["init", "runCallbacks", "useCrashHandler", "fileRead", "UGCRead"],
		:format => method(:create_lib_linux)
	},
	{
		:file => "src_linux/com/amanitadesign/steam/FRESteamWorks.as",
		:ignore => [],
		:format => method(:create_constants_linux),
		:start => "START GENERATED VALUES",
		:end => "END GENERATED VALUES"
	},
	{
		:file => "../src/FRESteamWorks/functions.h",
		:ignore => [],
		:format => method(:create_macro)
	}
]

contents = File.read("API.txt").split("\n")

files.each do |options|
	file_content = File.read(options[:file]).split("\n")

	start_marker = options[:start]||"START GENERATED CODE"
	end_marker = options[:end]||"END GENERATED CODE"
	start_idx = file_content.find_index{|line| line.include? start_marker }
	end_idx = file_content.find_index{|line| line.include? end_marker  }
	indentation = file_content[start_idx].gsub(/^(\s*).+$/, "\\1")

	func_num = -1
	replacement = contents.map do |line|
		next nil if line.empty?
		next "#{indentation}#{line}" if line[0].chr == "/"

		match = line.match /function ([^(]+)\(([^)]*)\):([^;]+)/
		($stderr.puts "Invalid line: #{line}"; next "/* invalid line */") unless match

		func_num += 1
		func, args, ret = match.captures
		next nil if options[:ignore].include? func

		func_name = "AIRSteam_#{func[0].chr.upcase + func[1,func.size]}"
		arg_names = args.split(/:[^,]+(?:,\s*)?/)
		options[:format].call(line, func_name, func_num, arg_names, ret).gsub(/^/, indentation)
	end.reject{|a|a.nil?}

	file_content[start_idx + 1..end_idx - 1] = replacement

	File.open(options[:file], "w") {|f|
		f.puts file_content.join("\n")
	}
end
