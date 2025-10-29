def main():
   driver_handle =open ('/proc/hello_driver_proc_read') 
   msg_from_kernel = driver_handle.readline()
   print(msg_from_kernel)
   return

main ()

