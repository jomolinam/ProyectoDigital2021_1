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
				 //read = 1;
				 addr_o = 0;
				 //image_select = !image_select;
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
//            if(HREF == 0 && VSYNC == 0)begin
//                status = capture;
//            end
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
