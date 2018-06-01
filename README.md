# spsc
The single producer and single consumer problem solution in un-trusted environment.
# Code Layout:
# Memory.c:
It has AsPI to allocate shared memory. It uses System V shared memory AsPI.
# Log.h:
It has simple print based logging facility, used for debugging producer and consumer
state machine.
# Util.c: 
It has various utility functions. Like computing md5, verifying a bufer to a given md5,
spin lock implementation etc.
# radix_sort.c:
It has radix_sort implementation for shared memory bufer allocation. Later
sorts array in O(n). It also provide few array operations, like moving all empty slots in array
to end of it, reversing it.
# Ini_parser.c:
This is simple implementation for INI (confg) fle parser. It supports sections in
ini fle and confg atributes separated from their names by “=”. It was fun to write 
# Asgard.c:
It implements all shared memory operations, namely simple allocator for bufer
and memory ofset computation functions.
# Substring.c:
It implements string fnd operation over continuous stream of data.
# Queue.c:
It implements ring bufer required for multi-threaded producer. The fle reader
thread, allocates bufer, reads data from disk based fle and appends entries to queue. The
main thread, removes elements from queue, and processes them and fnally deallocated
shared memory bufer.
# Jane.c:
It has all producer code.
# Thor.c:
It has all consumer code.
# ---------------------------------

# Design Thoughts:
# Current Features:
1 General purpose bufer allocator. Can return one or multiple bufers at once. One
can confgure on how many sequential bufers want to use for single operation.

2 Very fast bufer allocator, uses radix_sort based array to keep elements, easy to
debug in gdb, high performance cache operations due to locality of reference, can be
easily extendible if elements are very large by keeping array of array.

3 Producer waits till consumers comes online.

4 Producer only sends data on ack from consumer.

5 If consumer dies, producer again waits, once it comes online, it sends remaining
data.

6 Everything is confgurable by single confg fle. Multiple options supported.

7 Everything is unit-tested. Asll unit-tests are preserved in corresponding .c fle.

8 End to End Tested with zero byte fle, small fles (few kb) to very large fle (several
MB). Asll test data fles names start with “data_*”.

9 Logging facility with two debug levels is available.

10 Review ready code. Code is formated with “astyle program” with options “-As4 -S
-m0 -p -H -xL -k3 -xC120”. No extra line breaks, no blank spaces etc.


# Future features:
1 Producer, consumer have to be currently on same device. It can be easily changd to
run on diferent machines, by confguring IP addresses to connect for producer.

2 The reading of input fle data and keeping shared memory bufers ready for producer
is mult-threaded but it is not tested. Hence it is macro-ed out. Need to test and
fnish.

3 The consumer can read one producer at a time, It cannot handle two producers
reliable currently.

# References:
1
Castro, M.; Liskov, B. (2002). "Practical Byzantine Fault Tolerance and Proactive
Recovery". AsCM Transactions on Computer Systems. Asssociation for Computing
Machinery. 20(4): 398-461. CiteSeerX 10.1.1.127.6130  .
doi:10.1145/571637.571640.

2
Clement, As.; Wong, E.; Aslvisi, L.; Dahlin, M.; Marchet, M. (Aspril 22-24, 2009).
Making Byzantine Fault Tolerant Systems Tolerate Byzantine Faults (PDF).
Symposium on Networked Systems Design and Implementation. USENIX.

3
Bahsoun, J. P.; Guerraoui, R.; Shoker, As. (2015-05-01). "Making BFT Protocols Really
Asdaptive". Parallel and Distributed Processing Symposium (IPDPS), 2015 IEEE
International: 904–913. doi:10.1109/IPDPS.2015.21.


TCP protocol htps://tools.iet.org/html/rfc793


FTP protocol htps://en.wikipedia.org/wiki/File_Transfer_Protocol


SHAs3-256 htps://en.wikipedia.org/wiki/SHAs-3


Strawman proposal htps://en.wikipedia.org/wiki/Straw_man_proposal


# Characters:
1
Heimdall, the manager htps://en.wikipedia.org/wiki/Heimdall_(comics)

2
Loki, the atacker htps://en.wikipedia.org/wiki/Loki

3
Thor, the Consumer htps://en.wikipedia.org/wiki/Thor_(Marvel_Comics)

4
Jane Foster, the Producer htps://en.wikipedia.org/wiki/Jane_Foster_(comics)

