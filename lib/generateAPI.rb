#!/usr/bin/ruby1.9.1

def create_lib line, func
	<<-EOD
#{line} {
	return _ExtensionContext.call("#{func[:air_name]}"#{["", *func[:args].keys].join(", ")}) as #{func[:ret]};
}
	EOD
end

@lib_lin_defaults = Hash.new { |h,k| h[k] = ["readResponse() as #{k}", "null"] }
@lib_lin_defaults.merge!({
	"Boolean" => ["readBoolResponse()", "false"],
	"Number" => ["readFloatResponse()", "0.0"],
	"int" => ["readIntResponse()", "0"],
	"uint" => ["readIntResponse()", "0"],
	"String" => ["readStringResponse()", "\"\""]
})
def create_lib_linux line, func
	type, default = @lib_lin_defaults[func[:ret]]

	<<-EOD
#{line} {
	if(!callWrapper(#{func[:air_name]}, [#{func[:args].keys.join(", ")}])) return #{default};
	return #{type};
}
	EOD
end

files = [
	{
		:file => "src/com/amanitadesign/steam/ISteamWorks.as",
		:ignore => ["init"],
		:format => -> line, func { line.sub "public ", "" }
	},
	{
		:file => "src/com/amanitadesign/steam/FRESteamWorks.as",
		:ignore => ["init"],
		:format => method(:create_lib)
	},
	{
		:file => "src_linux/com/amanitadesign/steam/FRESteamWorks.as",
		:ignore => ["init", "runCallbacks", "useCrashHandler", "fileRead", "UGCRead",
			"getAuthSessionTicket", "beginAuthSession", "userHasLicenseForApp"],
		:format => method(:create_lib_linux)
	},
	{
		:file => "src_linux/com/amanitadesign/steam/FRESteamWorks.as",
		:ignore => [],
		:format => -> line, func { "private static const #{func[:air_name]}:int = #{func[:num]};" },
		:start => "START GENERATED VALUES",
		:end => "END GENERATED VALUES"
	},
	{
		:file => "../src/FRESteamWorks/functions.h",
		:ignore => [],
		:format => -> line, func { "X(#{func[:air_name]}) /* = #{func[:num]} */" }
	}
]

def parse_prototype line
	match = line.match /function ([^(]+)\(([^)]*)\):(\w+)$/
	raise "Invalid line: #{line}" unless match

	func, args, ret = match.captures

	arg_spec = args.scan(/(\w+):([^,]+)(?:,\s*)?/)
	arg_map = Hash[arg_spec]

	# We assume that insertion order is maintained when converting a hash to an
	# array. This is guaranteed by Ruby 1.9, but better be explicit about it.
	raise "Hash insertion order not maintained" unless arg_spec == arg_map.to_a

	return {
		:name => func,
		:air_name => "AIRSteam_#{func[0].chr.upcase + func[1,func.size]}",
		:args => arg_map,
		:ret => ret
	}
end

def generate_api contents, files
	files.each do |options|
		file_content = File.read(options[:file]).split("\n")

		start_marker = options[:start] || "START GENERATED CODE"
		end_marker = options[:end] || "END GENERATED CODE"
		start_idx = file_content.find_index{|line| line.include? start_marker }
		end_idx = file_content.find_index{|line| line.include? end_marker  }
		raise "Invalid indices: #{start_idx} - #{end_idx}" unless end_idx > start_idx

		indentation = file_content[start_idx].scan(/^\s*/)[0]

		func_num = -1
		replacement = contents.map do |line|
			next nil if line.empty?
			next "#{indentation}#{line}" if line[0].chr == "/"

			func_num += 1
			func = parse_prototype line
			func[:num] = func_num

			next nil if options[:ignore].include? func[:name]

			options[:format].call(line, func).gsub(/^/, indentation)
		end.reject{|a|a.nil?}

		file_content[start_idx + 1 .. end_idx - 1] = replacement

		File.open(options[:file], "w") {|f|
			f.puts file_content.join("\n")
		}
	end
end

contents = File.read("API.txt").split("\n")
generate_api contents, files
