module PI_ver(clk,rst_n,enable,set,feedback,A,B,result);
input clk,rst_n,enable;
input [31:0]set;
input [31:0]feedback;
output [31:0]result;
wire [31:0]result;
input [31:0] A;//A=Kp+Ki;
input [31:0] B;//B=Kp;
reg [31:0] Error;
reg [31:0] preError;
wire [31:0] P_buffer;
wire [31:0] I_buffer;
reg [31:0]A_t,B_t;
//reg [15:0]result_buffer;
reg [31:0]delta;
always @(posedge clk or negedge rst_n)
begin
if(!rst_n)
	begin
		A_t<= 32'd360;
		B_t<= 32'd210;
		Error<=32'sd0;
		preError<=32'sd0;
	end
else
	begin
		A_t<= A; 
		B_t<= B;
		if(enable==1'b1)
		begin
		    //if(set == 0)
		    //begin
		    //    A_t<= 100;
		    //    B_t<= 70;
		    //end
		    //else
		    //begin
		    //    A_t<= A; 
		    //    B_t<= B;
		    //end
		    preError<=Error;
		    if((set *128-feedback < 32) && (set *128-feedback > -32))
		        Error <= 0;
		    else
			    Error <= set *128-feedback;					
		end
		else ;
	end
end//end always

assign P_buffer=A_t*Error;
assign I_buffer=B_t*preError;
//LPM_multi LPM_multi_t1(A,Error,P_buffer);
//LPM_multi LPM_multi_t2(B,preError,I_buffer);

always @(P_buffer,I_buffer)
begin
	 delta<=P_buffer-I_buffer;	
end//end always
assign result=delta;
endmodule  