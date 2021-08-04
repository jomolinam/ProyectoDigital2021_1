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