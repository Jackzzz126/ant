server = {
	ip = "192.168.1.21",
	port = 8003,
	backlog = 128,
	daemon = 0,
	sockTimeout = 300,
	workerThreads = 5,
	dbThreads = 2
}

log = {
	logFile = "",
	errFile = ""
}

redis = {
	ip = "192.168.1.21",
	port = 6379,
	char = "Char_"
}

function check()
	local errStr = ""

	if server.ip == "" or server.port == 0 then
		errStr = errStr .. "Config error: ip, port invalid.\n";
	end

	if server.backLog == 0 then
		errStr = errStr .. "Config error: backlog invalid.\n";
	end

	if not server.daemon == 0 and (log.logFile == "" or log.errFile == "")then
		errStr = errStr .. "Config error: must set log file in daemon mode.\n";
	end

	if server.workerThreads < 1 then
		errStr = errStr .. "Config error: worker thread must > 1.\n";
	end

	if server.dbThreads < 1 then
		errStr = errStr .. "Config error: db thread must > 1.\n";
	end

	if redis.ip == "" or redis.port == 0 then
		errStr = errStr .. "Config error: db ip, port invalid.\n";
	end

	if redis.char == "" then
		errStr = errStr .. "Config error: db keys invalid.\n";
	end

	if errStr == "" then
		return 1, ""
	else
		return 0, errStr
	end
end

-- test
-- ret, errStr = check();
-- if ret == 0 then
-- 	print(errStr)
-- end




