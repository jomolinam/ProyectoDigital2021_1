# ProyectoDigital2021_1
Modulo Driver CAM
Este modulo es el encargado de la recepcion de los datos de imagen, por ello requiere tener como entradas el P_clk, D, VSYNC y HREF que son las señales qeu da la camara para una correcta sincronizacion y toma de datos. Ademas requiere la señal de car_stop para saber cuendo toma datos y cuando no.
Las salidas que tiene son regwrite para dar la indicacion de escribir en la memoria, data_w es el dato de tres bits que contiene la informacion en RGB de cada pixel, addr que es la direccion de escritura en la memoria y read que es la señal que indica cuando se debe leer la memoria para realizar el analisis del color. 
	module driver_cam#(
	parameter AW = 15
	)
	(
	input car_stop,
	input VSYNC,
	input HREF,
	input [7:0]D,
	input P,
	input rst,
	output reg regwrite,
	output reg [2:0] data_w,
	output wire [AW-1:0]addr,
	output reg read
	);



	reg [1:0] status = 0;
	reg all_stop = 0;
	reg count_pixel=1;
	reg caption = 0;
	reg [AW-1:0]addr_o = 0;
	reg image_select = 0;

Se debe verificar cuando caption y HREF estan en alto para saber que la informacioon de una fila de pixeles se debe tomar, adicionalmente se verifica que la direccion maxima dada por la cantidad de pixeles del frame no sea superada. 
Se utiliza count_pixel para saber si se estan tomando los datos del primer o del segundo byte de informacion de un pixel y dependiendo el caso se realiza una compuerta or entre los 3 bits mas significativos de cada color (RGB) para luego enviar esta informacion a la memoria. 

	always @(posedge P)begin
	    if(rst==0)begin
			if (caption && HREF && addr < 15'd25343)begin
				 count_pixel = count_pixel+1;
				 if(count_pixel==0)begin
					  data_w[2] = D[7] || D[6] || D[5];
					  data_w[1] = D[2] || D[1] || D[0];
				 end
				 if(count_pixel==1)begin
					  data_w[0] = D[4] || D[3] || D[2];
					  addr_o = addr_o+1;
					  regwrite = 1;
				 end
			end
			if(all_stop == 1)begin
				 data_w = 0;
				 addr_o = 0;
				 regwrite = 0;
				 read = 0;
			end
			if(all_stop == 0)begin
				 read = 1;
			end
			if(addr == 15'd25343)begin
				 addr_o = 0;
			end
			if(VSYNC == 1 && addr < 15'd25343)begin
				 addr_o = 0;
		end
	    end else begin
		read = 1;
		addr_o = 0;
		data_w = 0;
		regwrite = 0;
	    end
	  end

A continuacion se tiene la maquina de estados, se presentan tres estados, el primero es start en donde se mira si el carro esta quieto o no usando la señal de car_stop y asi avanzar a los demas estados. El segundo estado es capture, se entra en este estado si car_stop es igual a 1 y VSYNC es igual a cero y cuando esto sucede caption se hace igual a 1 indicando que se deben tomar datos y enviarlos a la memoria. El tercer estado es stop, se entra en este estado si car_stop es igual a cero y lo que hace es poner all_stop en 1 para frenar la toma de los datos, la escritura en memoria y ademas la lectura de la memoria, es un estado de espera hasta que el carro se detenga nuevamente.  

	    parameter start=0, capture=1, stop=2;
	    always @(negedge P)begin
		case(status)
		start:begin
		    caption = 0;
		    all_stop = 0;
		    if(car_stop == 1)begin
			if(VSYNC==0)begin
			    status = capture;
			end else begin
			    status = start;
			end
		    end else begin
			status = stop;
		    end
		end

		capture:begin
		    caption = 0;
		    if(VSYNC==0)begin
			caption=1;
			status = capture;
		    end
		    if (VSYNC == 1)begin
			status = start;
		    end 
		end

		stop:begin
		    caption = 0;
		    if(car_stop==0)begin
			all_stop = 1;
			status = stop;
		    end else begin
			status = start;
		    end
		end

		endcase
	    end
	assign addr = addr_o; 


	endmodule
	
Modulo BUffer Ram
Este modulo crea una memoria para guardar los datos que salen del modulo driver_cam 

	module buffer_ram#(
	parameter AW = 15, //bits de la direccion
	parameter DW = 3,
	parameter   imageFILE= "RGBimage.mem"  //bits de datos
	//parameter imagefile = "se necesita un archivo de inicializacion" //inicializacion de la ram
	)
	(
	input clk_w, //reloj de escritura
	input [AW-1:0] addr_in, //direccion de entrada
	input [DW-1:0] data_in, //dato de entrada
	input regwrite,
	input [AW-1:0] addr_out, //direccion de salida
	input rst,
	output [DW-1:0] data_out //dato de salida
	);
	reg [DW-1:0] out = 0;
	localparam VPOS = 2**AW; //tamaño vertical de la memoria
	reg [DW-1:0] ram [0:VPOS-1]; //memoria

	always @(posedge clk_w)begin
	     if(rst ==0)begin
		if (regwrite ==1)begin
		    ram[addr_in] <= data_in;
		end

		out <= ram[addr_out];
		//data_out_2 <= ram[addr_out_2];
	    end else begin
	    $readmemb(imageFILE, ram);
	    end
	end


	assign data_out = out;

	endmodule
	
Modulo Colors

	module colors#(
	parameter AW = 15
	)
	(
	input read,
	input [2:0] data,
	input P,
	input rst,
	output [AW-1:0] addr,
	output reg RED_out,
	output reg GREEN_out,
	output reg BLUE_out
	);

	reg [AW-1:0]addr_o = 0;
	reg [AW-1:0] RED = 0;
	reg [AW-1:0] GREEN = 0;
	reg [AW-1:0] BLUE = 0;
	assign addr = addr_o;


	always @(posedge P)begin
		if(read == 1)begin
			if(addr_o < 15'd25343)begin
				RED = RED + data[2];
				GREEN = GREEN + data[1];
				BLUE = BLUE + data[0];
				addr_o = addr_o + 1;
			end
				if(addr_o == 15'd25343)begin
				if(RED > GREEN && RED > BLUE)begin
					RED_out = 1;
					GREEN_out = 0;
					BLUE_out = 0;
				end else if(GREEN > RED && GREEN > BLUE)begin
					RED_out = 0;
					GREEN_out = 1;
					BLUE_out = 0;
				end else if(BLUE > RED && BLUE > GREEN)begin
					RED_out = 0;
					GREEN_out = 0;
					BLUE_out = 1;
				end else begin
					RED_out = 0;
					GREEN_out = 0;
					BLUE_out = 0;
				end
				addr_o = 0;
			end
		end else begin
		addr_o = 0;
		end
	end
	//initial begin 
	//    data = 0;
	//  end
	endmodule
	
Modulo Proyecto camara

	module proyectocamara#(
	parameter AW = 15, DW = 3
	)
	(
	input VSYNC,
	input HREF,
	input P,
	input [7:0] D,
	input car_stop,
	input clk,
	input rst,
	output X_clk,
	output wire RED_L,
	output wire GREEN_L,
	output wire BLUE_L,
	output wire RED_P,
	output wire GREEN_P,
	output wire BLUE_P
	);

	wire regwrite;
	wire [DW-1:0] data_w;
	wire [AW-1:0] addr_w;
	wire read;
	wire [DW-1:0] data_r;
	wire [AW-1:0] addr_r;
	wire clk_r;
	wire RED;
	wire GREEN;
	wire BLUE;

	//wire image_s;
	//wire [DW-1:0] data_2;
	//wire [AW-1:0] addr_2;

	//assign X_clk=clk_r; 


	clk_wiz_0 divisordefrecuencia(
	.clk_in1(clk),
	.clk_out1(X_clk)
	);

	driver_cam driver_cam(
	.car_stop(car_stop),
	.VSYNC(VSYNC),
	.HREF(HREF),
	.D(D),
	.P(P),
	.rst(rst),
	.regwrite(regwrite),
	.data_w(data_w),
	.addr(addr_w),
	.read(read)
	);

	buffer_ram buffer_ram(
	.clk_w(P),
	.addr_in(addr_w), 
	.data_in(data_w), 
	.regwrite(regwrite),
	.addr_out(addr_r), 
	.rst(rst),
	.data_out(data_r)
	);

	colors colors (
	.read(read),
	.data(data_r),
	.P(P),
	.rst(rst),
	.addr(addr_r),
	.RED_out(RED),
	.GREEN_out(GREEN),
	.BLUE_out(BLUE)
	);


	assign RED_L = RED;
	assign GREEN_L = GREEN;
	assign BLUE_L = BLUE;
	assign RED_P = RED;
	assign GREEN_P = GREEN;
	assign BLUE_P = BLUE;

	endmodule
