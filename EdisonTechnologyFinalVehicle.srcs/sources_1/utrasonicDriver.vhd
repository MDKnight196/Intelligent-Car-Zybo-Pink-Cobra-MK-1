----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 11/08/2016 01:10:10 PM
-- Design Name: 
-- Module Name: utrasonicDriver - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.ALL;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity utrasonicDriver is
    GENERIC (CONSTANT max : integer := 88800); --1.0 Hz, creates a 1Hz clock from 100MHz system clock
    Port ( iCLK : in STD_LOGIC;
           echo : in STD_LOGIC;
           trig : out STD_LOGIC;
           pluseWidth : out STD_LOGIC_VECTOR (7 downto 0));
end utrasonicDriver;

architecture Behavioral of utrasonicDriver is

signal clk_cnt: integer range 0 to max;
signal sTrig : std_logic := '0';
signal sWidth : integer := 0;
signal echo_cnt :integer := 0;
begin

process(iCLK)
begin
     if rising_edge(iCLK)  then
            if (clk_cnt >= max and sTrig = '0') then
                clk_cnt <= 0; --reset clk_cnt to 0
                sTrig <= '1'; --Make clk_en high
            elsif (clk_cnt >= 1000 and sTrig = '1') then
                clk_cnt <= 0; --reset clk_cnt to 0
                sTrig <= '0'; --Make clk_en high
            else
                clk_cnt <= clk_cnt + 1; --if not cnt_max add 1 to clk_cnt
            end if;
	   end if;
	
end process;

process(echo, iCLK)
begin
     if rising_edge(iCLK)then
        if echo = '1' then 
            echo_cnt <= echo_cnt + 1;
        elsif (echo_cnt > 0 and echo = '0') then
            sWidth <= (echo_cnt/100)/148;
            echo_cnt <= 0;
        else 
            echo_cnt <= 0;
        end if;
	 end if;
	
end process;

trig<=sTrig;
pluseWidth <= std_logic_vector(to_unsigned(sWidth, pluseWidth'length));
end Behavioral;
