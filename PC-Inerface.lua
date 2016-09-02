--include modules
rs232 = require("luars232")

port_name = "/dev/ttyUSB0"
timeout = 1000      --1000 msecs


--open port
local e, p = rs232.open(port_name)
if e ~= rs232.RS232_ERR_NOERROR then
	-- handle error
	io.stderr:write(string.format("can't open serial port '%s', error: '%s'\n",port_name, rs232.error_tostring(e)))
	return
end
--io.write(string.format("\n'%s'\n", tostring(p)))
-- set port settings
assert(p:set_baud_rate(rs232.RS232_BAUD_9600) == rs232.RS232_ERR_NOERROR,"error in baud rate setting")
assert(p:set_data_bits(rs232.RS232_DATA_8) == rs232.RS232_ERR_NOERROR,"error in data bits setting")
assert(p:set_parity(rs232.RS232_PARITY_NONE) == rs232.RS232_ERR_NOERROR,"error in parity setting")
assert(p:set_stop_bits(rs232.RS232_STOP_1) == rs232.RS232_ERR_NOERROR,"error in stop bits setting")
assert(p:set_flow_control(rs232.RS232_FLOW_OFF)  == rs232.RS232_ERR_NOERROR,"error in flow control setting")
io.stderr:write(string.format("\nOK, port open with values '%s'\n", tostring(p)))


-- open file
print("\ninput hex-file filename\n")
filename = "test.hex"--io.read()
f = assert( io.open(filename,"r"), "\nerror opening file\n")



--functions to be used
function byteFromHex (str)
    local x = {}
    --local i=1
    for y in str:gmatch('(..)') do
        --print("y=",y)
        x[#x+1] = string.char( tonumber(y,16) )         --- output is always converted to corresponding ascii
        --print("x=",x[i],i)
        --i=i+1
    end

    return table.concat(x)
end

function hexFromByte(str)
    local x = {}
    for y in str:gmatch('(.)') do
        x[#x+1] = string.format('%02X', string.byte(y))
    end
    return table.concat(x)
end



function TxRxPacket (command_byte, pack_size, txPacket, ack_size )
    --print("\nsending command byte")
    print("send...", command_byte, string.len(command_byte) )       ------- always send char type data as it is received as uns char at uart
    local err, len_written = p:write(command_byte)
    assert(err == rs232.RS232_ERR_NOERROR,"\nerror sending command_byte\n")
    --wait for reply
    local err, rxPacket, size = p:read(1,timeout,1)     ---------reads ascii value of what is sent on uart
    print("received command byte=",rxPacket,"size=", size) 
    if rxPacket ~= command_byte then
        print("\nrouter not responding\n")
    end

    --if write buffer, send size to be written else not...
    if command_byte=='t' then
        print("send txpack size...",pack_size, string.len(pack_size))
        err, len_written = p:write(pack_size)
        err, rxPacket, size = p:read(5,timeout,1)
        print("received size=",rxPacket,"size=", size)
    end
       
    --send packet
    if (txPacket ~= nil) then
        print("\nsending packet without timeout", hexFromByte(txPacket),string.len(txPacket))
        err, len_written = p:write(txPacket)
        assert(err == rs232.RS232_ERR_NOERROR,"\nerror sending packet\n")
    end    
    
    --wait for acknowledgement
    err, rxPacket, size = p:read(ack_size,timeout,1)
    if err ~= rs232.RS232_ERR_NOERROR then print("\nerror receiving packet\n") end
    print("read packet=",hexFromByte(rxPacket),"size=",string.len(rxPacket))
    --print("read packet=",rxPacket,"size=",size)
    --print(rxPacket[1], string.sub(rxPacket,1,1) )--, string.char(rxPacket[1]) )
    if (rxPacket == nil or (string.sub(rxPacket,1,1) ~= 'U') ) then        --ack pack is 0x55
            print("no ACK received")
    end

    return rxPacket
end

        

--main program --main sequence

    
    ---------------------------FILE DETAILS---------------------------------------------
    --if command == "" then 
    --print entire file
        f:seek("set")
        print("\nfile details:\n" .. f:read("*a"))
       
        --check no of character in a line
        f:seek("set")
        local line = f:read("*l")
        charCount = string.len(line)
        charCount = charCount+1              --for '\n'
        print ("no of characters in a line including : is", charCount)              
        assert(charCount == 45,"cannot process, exiting........")        --length must be 45 43data+2 invisible bytes
        
         
while true do
    print("\n \nt =temp Pg Buff","f=flash write", "r=read flash","v=verify flash","s=tatus check","j=jump to app","e=exit...") 
    print("waiting for input command")
    command=io.read()
    print(string.format("command= %s,   length= %d",command,string.len(command)) )

    --------------------------WRITE TEMP PAGE------------------------------------------------
    if command == "t" then
        print("provide line no of line to be written to temp buffer: ")
        local line_no = io.read()

        --extract a line from file
        f:seek( "set", ((line_no-1)*charCount) )
        local line = f:read("*l")
        print("extracted line for line no", line_no,"is:","\n"..line)
        
        local pack_size = string.sub(line,2,3)
        --print(pack_size)
        pack_size = byteFromHex(pack_size)
        --print("pack_size=", pack_size, string.len(pack_size), string.char(16))

        --modify packet = fill buffer byte + rest 16 bytes
        local txBuffer = string.sub(line,10,-4)
        --print("tmpPacket=", txBuffer)
        txBuffer = byteFromHex(txBuffer)
        --print("packet to be sent is:",hexFromByte(txBuffer) )  --display packet
        
        --send command + packet and wait for ack and display
        local rxBuffer = TxRxPacket('t',pack_size, txBuffer,2)
        print("temp Page writing successful!!! \ntemp Page Buffer Position=?",hexFromByte(string.sub(rxBuffer,2,2)) )    --1st= ACK, 2nd=pos
         
        

    ----------------------------------------WRITE FLASH------------------------------------
    elseif command == "f" then
        --send write flash command and FlashAddr
        print("provide FlashAddress where TempBuffer Page is to be burnt\n")
        local FlashAddr = tostring(io.read())
        print(FlashAddr, string.len(FlashAddr))
        FlashAddr = byteFromHex(FlashAddr)
        print( FlashAddr, string.len(FlashAddr) )

        --send packet and wait for ack
        txBuffer = FlashAddr
        rxBuffer = TxRxPacket('f',pack_size,txBuffer,3)
        print("write to flash succesful!!! \ntemp Page Buffer Position=?",hexFromByte(string.sub(rxBuffer,2,2)) ,"\nFlashAddr Position=?", hexFromByte(string.sub(rxBuffer,3,4)) )


    -------------------------------------READ FLASH----------------------------------------
    elseif command == "r" then
        print("\nprovide 2byte FlashAddr to read")     
        FlashAddr = tostring(io.read())           --scan for 2 byte adress
        
        --send read command + adress
        txBuffer = byteFromHex(FlashAddr)
        --print(txBuffer,string.len(txBuffer))
        rxBuffer = TxRxPacket('r',pack_size,txBuffer,19)      --1=ACK, 2-17=flashbytes, 18&19-FlashAddr
        print("16 consecutive bytes from the Flash Address provided is :\n",hexFromByte(string.sub(rxBuffer,2,17)),"\nFlashAddr=",hexFromByte(string.sub(rxBuffer,18,19)) )
        

    -------------------------------------VERIFY FLASH-----------------------------------       
    elseif command == "v" then
        print("\ninput hex file line_no & flash adress to be verified\n")  
        line_no = io.read()         --scan for line no & address to be verified
        local FlashAddr = tostring(io.read())
       
        --form & appropriate packet (include CRC +flashaddr to be verified)
        f:seek( "set", ((line_no-1)*charCount) )
        line = f:read("*l")
        print(line)

        pack_size = string.sub(line,2,3)
        --print(pack_size)
        pack_size = byteFromHex(pack_size)
        local CRC = string.sub(line,-3,-2)
        print(CRC)
        CRC = byteFromHex(CRC)
        print (CRC)
        FlashAddr = byteFromHex(FlashAddr)

        txBuffer = FlashAddr .. pack_size .. CRC         --2+1+1=4
        print(txBuffer,string.len(txBuffer))
        rxBuffer = TxRxPacket('v',pack_size,txBuffer,4)     --1=ACK, 2=yesOK?, 3&4=FlashAddr
        print("verification OK=?", hexFromByte(string.sub(rxBuffer,2,2)),"\tFlashAddr verified=?", string.sub(rxBuffer3,4));--display ack yes or no
             

    -------------------------------------CHECK STATUS------------------------------------
    elseif command == "s" then
        --send packet for checking status & wait and dispplay ack pack
        local rxBuffer = TxRxPacket('s',pack_size,nil,4)
        print("status:\ntmp Pg Buff Pos=",hexFromByte(string.sub(rxBuffer,2,2)), "FlashAddr Pos=", hexFromByte(string.sub(rxBuffer,3,4)) )
        

    -----------------------------------JUMP TO APP---------------------------------------        
    elseif command == "j" then
        --send jump packet & wait and display ack
        local rxBuffer = TxRxPacket('j',pack_size,nil,1)
        if (string.sub(rxBuffer,1,1) == 'U') then 
            print("process complete... exiting.............\n\n" )
            f:close()
            p:close()
            return   
        end
       
    -----------------------------------QUIT-----------------------------------------------
    elseif command == "e" then
        print("exiting...............\n\n")
        f:close()
        p:close()
        return
    
    
    ---------------------------------INVLAID INPUT-----------------------------------------
    else 
        print("invalid input..........\n")
        
    end

end     

