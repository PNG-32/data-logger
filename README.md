# Data Logger CHRONO

![Data Logger CHRONO](docs/datalogger.png "Data Logger CHRONO")

[Manual](docs/manual.pdf)

[Video]()

## Commands

```
@[set, s]
	[dt, d] <ISO-Formatted Date>
	[radj, ra, a] <min> <max>
	[ldr, l] <min> <max>
	[hum, h] <min> <max>
	[temp, t] <min> <max> <{c}, f, k>
	[zone, z] <hour>:<minute>
	[unit, u] <c, f, k>

@[read, r]
	{sensors}
	rawldr
	
@[wipe, w]
	log
	absolutelyeverything

@[view, v]
	[log, l]
	[zone, z]
	[thresh, t]
	[radj, ra, a]
```

### Examples

```
	@view log
	@v log
	@view l
	@v l
	
	@read sensors
	@r sensors
	@read
	@r
	
	@wipe log
	@w log
	
	@set zone -3:00
	@s zone -3:00
	@s z -3:00
```
