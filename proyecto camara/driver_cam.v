module driver_cam#(
parameter AW = 15
)
(
input car_stop,
input VSYNC,
input HREF,
input [7:0]D,
input P_clk,
//output rst,
//output PWDN,
output reg regwrite,
output reg [2:0] data_w,
output reg [AW-1:0]addr,
output reg read
);


reg [1:0] status = 0;
reg all_cero = 0;
reg count_pixel=1;
reg caption = 0;
//reg [AW-1:0]addr_o=0; 
//assign addr = addr_o-1;

reg car_stop_prev;

always @(posedge P_clk)begin
	if(car_stop_prev != car_stop)begin
		addr=0;
		car_stop_prev = car_stop;
	end else begin
		regwrite = 0;
		if (caption && addr < 15'd25343)begin
			count_pixel = count_pixel+1;
			read = 0;
			if(count_pixel==0)begin
				data_w[2] = D[7] || D[6] || D[5];
				data_w[1] = D[2] || D[1] || D[0];
			end
			if(count_pixel==1)begin
				data_w[0] = D[4] || D[3];
				addr = addr+1;
				regwrite = 1;
			end
		end
		if(all_cero == 1 && addr < 15'd25343)begin
			data_w = 0;
			addr = addr+1;
			regwrite = 1;
			read = 0;
		end
		if(addr == 15'd25344)begin
			regwrite = 0;
			read = 1;
		end
		if(VSYNC == 1 && addr < 15'd25343)begin
			addr = 0;
		end
	end
end

parameter start=0, capture=1, ceros=2;
always @(negedge P_clk)begin
	case(status)
	start:begin
		caption = 0;
		all_cero = 0;
		if(car_stop == 1)begin
			if(VSYNC==0 && HREF==1)begin
				status = capture;
			end else begin
				status = start;
			end
		end else begin
			status = ceros;
		end
	end
	
	capture:begin
		caption = 0;
		if(HREF==1)begin
			caption=1;
			status = capture;
		end
		if(HREF == 0 && VSYNC == 0)begin
			status = capture;
		end
		if (VSYNC == 1)begin
			status = start;
		end 
	end
	
	ceros:begin
		caption = 0;
		if(car_stop==0)begin
			all_cero = 1;
			status = ceros;
		end else begin
			status = start;
		end
	end
	
	endcase
end
endmodule
