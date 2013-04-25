#!/usr/bin/ruby1.9.1

contents = open("API.txt", "r").read.split("\n")

contents.each do |line|
	if line.empty? or line[0] == "/"
		puts "\t\t#{line}".rstrip
		next
	end

	func = line.split("(")[0].split[-1]
	ret = line.split(":")[-1]
	args = line[/\(([^)]*)\)/, 1]
	arg_names = args.split(/,\s*/).map do |arg|
		arg.split(":")[0]
	end.unshift("\"AIRSteam_#{func}\"")
	puts <<EOD
		#{line.sub(/[A-Z]/){|i|i.downcase}}
		{
			return _ExtensionContext.call(#{arg_names.join(", ")}) as #{ret};
		}

EOD

end
