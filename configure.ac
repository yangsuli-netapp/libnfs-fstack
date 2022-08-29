AC_PREREQ(2.50)
AC_INIT([libnfs], [5.0.2], [ronniesahlberg@gmail.com])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
AC_CANONICAL_HOST
AC_CONFIG_MACRO_DIR([m4])
m4_pattern_allow([AM_PROG_AR])
AM_PROG_AR

# Work around stupid autoconf default cflags. pt 1
SAVE_CFLAGS="x${CFLAGS}"

AC_PROG_CC
AC_PROG_LIBTOOL

AM_PROG_CC_C_O
PKG_PROG_PKG_CONFIG

# Work around stupid autoconf default cflags. pt 2
if test "$SAVE_CFLAGS" = "x"; then
  CFLAGS=""
fi

# We always want 64 bit file offsets
AC_SYS_LARGEFILE

#option: utils
MAYBE_UTILS="utils"
AC_ARG_ENABLE([utils],
              [AC_HELP_STRING([--enable-utils],
                              [Build util programs])],
	      [if test $enableval = no ; then
	    	    MAYBE_UTILS=""
	       fi])
AC_SUBST(MAYBE_UTILS)

#option fstack
AC_ARG_ENABLE([fstack],
	      [AC_HELP_STRING([--enable-fstack],
			      [Use f-stack user space networking])],
	      [ENABLE_FSTACK=$enableval],
	      [ENABLE_FSTACK="no"])

#option: examples
AC_ARG_ENABLE([examples],
              [AC_HELP_STRING([--enable-examples],
                              [Build example programs])],
	      [ENABLE_EXAMPLES=$enableval],
	      [ENABLE_EXAMPLES="no"])

# We need popt to compile the examples
if test x$ENABLE_EXAMPLES = xyes; then
AC_MSG_CHECKING(whether libpopt is available)
ac_save_LIBS="$LIBS"
LIBS="$LIBS -lpopt"
AC_TRY_LINK([#include <popt.h>], [
/*
 * Just see if we can compile/link with popt
 */
	int _argc;
	char **_argv;
	struct poptOption popt_options[] = {
		POPT_TABLEEND
	};
	poptGetContext(_argv[0], _argc, _argv, popt_options, POPT_CONTEXT_KEEP_FIRST);
], ac_cv_have_popt=yes, ac_cv_have_popt=no)
LIBS="$ac_save_LIBS"
if test "$ac_cv_have_popt" = yes ; then
  AC_MSG_RESULT(yes)
  MAYBE_EXAMPLES="examples"
else
  AC_MSG_RESULT(no)
  AC_MSG_NOTICE(You need libpopt to compile the sample libnfs clients.)
  AC_MSG_NOTICE(Only the library will be compiled and installed.)
fi
fi

AC_SUBST(MAYBE_EXAMPLES)

#option: examples
AC_ARG_ENABLE([pthread],
              [AC_HELP_STRING([--enable-pthread],
                              [Build with pthread multithreading support])],
	      [ENABLE_PTHREAD=$enableval],
	      [ENABLE_PTHREAD="no"])
if test x$ENABLE_PTHREAD = xyes; then
# check for lpthread
AC_CACHE_CHECK([for pthread support],libnfs_cv_HAVE_PTHREAD,[
AC_TRY_COMPILE([
#include <pthread.h>],
[pthread_t thread1, thread2;],
libnfs_cv_HAVE_PTHREAD=yes,libnfs_cv_HAVE_PTHREAD=no)])
if test x"$libnfs_cv_HAVE_PTHREAD" = x"yes"; then
    AC_DEFINE(HAVE_PTHREAD,1,[Whether we have pthread support])
    AC_DEFINE(HAVE_MULTITHREADING,1,[Whether we have multithreading support])
fi
fi
AM_CONDITIONAL([HAVE_PTHREAD], [test x$libnfs_cv_HAVE_PTHREAD = xyes])

AC_MSG_CHECKING(whether SO_BINDTODEVICE is available)
AC_TRY_COMPILE([#include <net/if.h>], [
        int i = SO_BINDTODEVICE;
], ac_cv_have_so_bindtodevice=yes, ac_cv_have_so_bindtodevice=no)
if test "$ac_cv_have_so_bindtodevice" = yes ; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_SO_BINDTODEVICE, 1, [Whether our sockets support SO_BINDTODEVICE])
else
  AC_MSG_RESULT(no)
  AC_MSG_NOTICE(SO_BINDTODEVICE support for if= support is missing.)
  AC_MSG_NOTICE(Compiling without if= support.)
fi

AC_MSG_CHECKING(whether getpwnam() is available)
AC_TRY_COMPILE([
	#include <sys/types.h>
	#include <pwd.h>
], [
        struct passwd *pwd = getpwnam("nobody");
], ac_cv_have_getpwnam=yes, ac_cv_have_getpwnam=no)
if test "$ac_cv_have_getpwnam" = yes ; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_GETPWNAM, 1, [Whether we have getpwnam()])
else
  AC_MSG_RESULT(no)
  AC_MSG_NOTICE(getpwnam() support is missing.)
  AC_MSG_NOTICE(Compiling without getpwnam() support.)
fi

AC_ARG_ENABLE([werror], [AS_HELP_STRING([--disable-werror],
              [Disables building with -Werror by default])])

if test "$ac_cv_prog_gcc" = yes; then
   WARN_CFLAGS="-Wall -Wshadow -Wno-write-strings -Wstrict-prototypes -Wpointer-arith -Wcast-align -Wno-strict-aliasing"
   if test "x$enable_werror" != "xno"; then
       WARN_CFLAGS="$WARN_CFLAGS -Werror"
   fi
fi
AC_SUBST(WARN_CFLAGS)

LIBSOCKET=
SYS=

case $host in
  *solaris*)
    AC_CHECK_HEADERS([sys/filio.h])
    AC_CHECK_HEADERS([sys/sockio.h])
    AC_CHECK_LIB([socket], [main], , [AC_MSG_ERROR([Can not find required library])])
    AC_CHECK_LIB([nsl],    [main], , [AC_MSG_ERROR([Can not find required library])])
    ;;
  *mingw32* | *wince* | *mingwce*)
    LIBSOCKET='-lws2_32'
    SYS=mingw32
    ;;
  *)
    ;;
esac
AM_CONDITIONAL(HAVE_WIN32,   test "${SYS}" = "mingw32")
AC_SUBST([LIBSOCKET])

# check for sys/sysmacros.h
dnl Check for sys/sysmacros.h
AC_CHECK_HEADERS([sys/sysmacros.h])

# check for poll.h
dnl Check for poll.h
AC_CHECK_HEADERS([poll.h])

# check for unistd.h
dnl Check for unistd.h
AC_CHECK_HEADERS([unistd.h])

# check for netdb.h
dnl Check for netdb.h
AC_CHECK_HEADERS([netdb.h])

# check for utime.h
dnl Check for utime.h
AC_CHECK_HEADERS([utime.h])

# check for net/if.h
dnl Check for net/if.h
AC_CHECK_HEADERS([net/if.h])

# check for sys/time.h
dnl Check for sys/time.h
AC_CHECK_HEADERS([sys/time.h])

# check for sys/ioctl.h
dnl Check for sys/ioctl.h
AC_CHECK_HEADERS([sys/ioctl.h])

# check for sys/vfs.h
dnl Check for sys/vfs.h
AC_CHECK_HEADERS([sys/vfs.h])

# check for sys/statvfs.h
dnl Check for sys/statvfs.h
AC_CHECK_HEADERS([sys/statvfs.h])

# check for fuse.h
dnl Check for fuse.h
AC_CHECK_HEADERS([fuse.h])

# check for sys/socket.h
dnl Check for sys/socket.h
AC_CHECK_HEADERS([sys/socket.h])

# check for netinet/tcp.h
dnl Check for netinet/tcp.h
AC_CHECK_HEADERS([netinet/tcp.h])

# check for netinet/in.h
dnl Check for netinet/in.h
AC_CHECK_HEADERS([netinet/in.h])

# check for arpa/inet.h
dnl Check for arpa/inet.h
AC_CHECK_HEADERS([arpa/inet.h])

# check for SA_LEN
dnl Check if sockaddr data structure includes a "sa_len"
AC_CHECK_MEMBER([struct sockaddr.sa_len],
                [ AC_DEFINE(HAVE_SOCKADDR_LEN,1,[Whether sockaddr struct has sa_len]) ],
                [],
                [
#include <sys/types.h>
#include <sys/socket.h>
])

# check for sockaddr_storage
dnl Check if sockaddr structure includes a "ss_family"
AC_CHECK_MEMBER([struct sockaddr_storage.ss_family],
                [ AC_DEFINE(HAVE_SOCKADDR_STORAGE,1,[Whether we have sockaddr_Storage]) ],
                [],
                [
#include <sys/types.h>
#include <sys/socket.h>
])

# check for clock_gettime(CLOCK_MONOTONIC_COARSE)
AC_MSG_CHECKING(if clock_gettime(CLOCK_MONOTONIC_COARSE) is available)
AC_TRY_COMPILE([#include <time.h>], [
        int i = clock_gettime(CLOCK_MONOTONIC_COARSE, NULL);
], ac_cv_have_clock_gettime=yes, ac_cv_have_clock_gettime=no)
if test "$ac_cv_have_clock_gettime" = yes ; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_CLOCK_GETTIME, 1, [Whether we have clock_gettime])
else
  AC_MSG_RESULT(no)
  AC_MSG_NOTICE(clock_gettime(CLOCK_MONOTONIC_COARSE) support is missing.)
  AC_MSG_NOTICE(Compiling without clock_gettime support..)
fi

# check for tevent + talloc
AC_CACHE_CHECK([for talloc and tevent support],libnfs_cv_HAVE_TALLOC_TEVENT,[
AC_TRY_COMPILE([
#include <talloc.h>
#include <tevent.h>],
[struct tevent_context *ctx = tevent_context_init(NULL);
 int major = talloc_version_major();],
libiscsi_cv_HAVE_TALLOC_TEVENT=yes,libiscsi_cv_HAVE_TALLOC_TEVENT=no)])
if test x"$libiscsi_cv_HAVE_TALLOC_TEVENT" = x"yes"; then
    AC_DEFINE(HAVE_TALLOC_TEVENT,1,[Whether we have talloc nad tevent support])
fi
AM_CONDITIONAL([HAVE_TALLOC_TEVENT], [test $libiscsi_cv_HAVE_TALLOC_TEVENT = yes])

if test x$ENABLE_FSTACK = xyes; then
AC_DEFINE(USE_FSTACK, 1, [Use fstack user space networking])
AC_CHECK_LIB([dpdk], [main], [AC_MSG_ERROR([Can not find required library])])
CFLAGS="$CFLAGS -gdwarf-2 -include rte_config.h -march=native -I/usr/local/include -I/usr/include/libnl3"
LIBS="$LIBS -L/usr/local/lib/x86_64-linux-gnu -Wl,--whole-archive -l:librte_common_cpt.a -l:librte_common_dpaax.a -l:librte_common_iavf.a -l:librte_common_octeontx.a -l:librte_common_octeontx2.a -l:librte_common_sfc_efx.a -l:librte_bus_dpaa.a -l:librte_bus_fslmc.a -l:librte_bus_ifpga.a -l:librte_bus_pci.a -l:librte_bus_vdev.a -l:librte_bus_vmbus.a -l:librte_common_mlx5.a -l:librte_common_qat.a -l:librte_mempool_bucket.a -l:librte_mempool_dpaa.a -l:librte_mempool_dpaa2.a -l:librte_mempool_octeontx.a -l:librte_mempool_octeontx2.a -l:librte_mempool_ring.a -l:librte_mempool_stack.a -l:librte_net_af_packet.a -l:librte_net_ark.a -l:librte_net_atlantic.a -l:librte_net_avp.a -l:librte_net_axgbe.a -l:librte_net_bond.a -l:librte_net_bnx2x.a -l:librte_net_bnxt.a -l:librte_net_cxgbe.a -l:librte_net_dpaa.a -l:librte_net_dpaa2.a -l:librte_net_e1000.a -l:librte_net_ena.a -l:librte_net_enetc.a -l:librte_net_enic.a -l:librte_net_failsafe.a -l:librte_net_fm10k.a -l:librte_net_i40e.a -l:librte_net_hinic.a -l:librte_net_hns3.a -l:librte_net_iavf.a -l:librte_net_ice.a -l:librte_net_igc.a -l:librte_net_ixgbe.a -l:librte_net_kni.a -l:librte_net_liquidio.a -l:librte_net_memif.a -l:librte_net_mlx4.a -l:librte_net_mlx5.a -l:librte_net_netvsc.a -l:librte_net_nfp.a -l:librte_net_null.a -l:librte_net_octeontx.a -l:librte_net_octeontx2.a -l:librte_net_pcap.a -l:librte_net_pfe.a -l:librte_net_qede.a -l:librte_net_ring.a -l:librte_net_sfc.a -l:librte_net_softnic.a -l:librte_net_tap.a -l:librte_net_thunderx.a -l:librte_net_txgbe.a -l:librte_net_vdev_netvsc.a -l:librte_net_vhost.a -l:librte_net_virtio.a -l:librte_net_vmxnet3.a -l:librte_raw_dpaa2_cmdif.a -l:librte_raw_dpaa2_qdma.a -l:librte_raw_ioat.a -l:librte_raw_ntb.a -l:librte_raw_octeontx2_dma.a -l:librte_raw_octeontx2_ep.a -l:librte_raw_skeleton.a -l:librte_crypto_bcmfs.a -l:librte_crypto_caam_jr.a -l:librte_crypto_dpaa_sec.a -l:librte_crypto_dpaa2_sec.a -l:librte_crypto_nitrox.a -l:librte_crypto_null.a -l:librte_crypto_octeontx.a -l:librte_crypto_octeontx2.a -l:librte_crypto_scheduler.a -l:librte_crypto_virtio.a -l:librte_compress_octeontx.a -l:librte_compress_zlib.a -l:librte_regex_mlx5.a -l:librte_regex_octeontx2.a -l:librte_vdpa_ifc.a -l:librte_vdpa_mlx5.a -l:librte_event_dlb.a -l:librte_event_dlb2.a -l:librte_event_dpaa.a -l:librte_event_dpaa2.a -l:librte_event_octeontx2.a -l:librte_event_opdl.a -l:librte_event_skeleton.a -l:librte_event_sw.a -l:librte_event_dsw.a -l:librte_event_octeontx.a -l:librte_baseband_null.a -l:librte_baseband_turbo_sw.a -l:librte_baseband_fpga_lte_fec.a -l:librte_baseband_fpga_5gnr_fec.a -l:librte_baseband_acc100.a -l:librte_node.a -l:librte_graph.a -l:librte_bpf.a -l:librte_flow_classify.a -l:librte_pipeline.a -l:librte_table.a -l:librte_port.a -l:librte_fib.a -l:librte_ipsec.a -l:librte_vhost.a -l:librte_stack.a -l:librte_security.a -l:librte_sched.a -l:librte_reorder.a -l:librte_rib.a -l:librte_regexdev.a -l:librte_rawdev.a -l:librte_pdump.a -l:librte_power.a -l:librte_member.a -l:librte_lpm.a -l:librte_latencystats.a -l:librte_kni.a -l:librte_jobstats.a -l:librte_ip_frag.a -l:librte_gso.a -l:librte_gro.a -l:librte_eventdev.a -l:librte_efd.a -l:librte_distributor.a -l:librte_cryptodev.a -l:librte_compressdev.a -l:librte_cfgfile.a -l:librte_bitratestats.a -l:librte_bbdev.a -l:librte_acl.a -l:librte_timer.a -l:librte_hash.a -l:librte_metrics.a -l:librte_cmdline.a -l:librte_pci.a -l:librte_ethdev.a -l:librte_meter.a -l:librte_net.a -l:librte_mbuf.a -l:librte_mempool.a -l:librte_rcu.a -l:librte_ring.a -l:librte_eal.a -l:librte_telemetry.a -l:librte_kvargs.a -Wl,--no-whole-archive -lmlx5 -lpthread -lz -lmlx4 -lpthread -libverbs -lbnxt_re-rdmav25 -lcxgb4-rdmav25 -lefa -lhns-rdmav25 -li40iw-rdmav25 -lmlx4 -lmlx5 -lmthca-rdmav25 -locrdma-rdmav25 -lqedr-rdmav25 -lvmw_pvrdma-rdmav25 -lhfi1verbs-rdmav25 -lipathverbs-rdmav25 -lrxe-rdmav25 -lsiw-rdmav25 -libverbs -lpthread -lnl-route-3 -lnl-3 -lpcap -Wl,--as-needed -lrte_node -lrte_graph -lrte_bpf -lrte_flow_classify -lrte_pipeline -lrte_table -lrte_port -lrte_fib -lrte_ipsec -lrte_vhost -lrte_stack -lrte_security -lrte_sched -lrte_reorder -lrte_rib -lrte_regexdev -lrte_rawdev -lrte_pdump -lrte_power -lrte_member -lrte_lpm -lrte_latencystats -lrte_kni -lrte_jobstats -lrte_ip_frag -lrte_gso -lrte_gro -lrte_eventdev -lrte_efd -lrte_distributor -lrte_cryptodev -lrte_compressdev -lrte_cfgfile -lrte_bitratestats -lrte_bbdev -lrte_acl -lrte_timer -lrte_hash -lrte_metrics -lrte_cmdline -lrte_pci -lrte_ethdev -lrte_meter -lrte_net -lrte_mbuf -lrte_mempool -lrte_rcu -lrte_ring -lrte_eal -lrte_telemetry -lrte_kvargs -pthread -lm -ldl -lnuma -lpcap -L/home/ubuntu/f-stack/lib -Wl,--whole-archive,-lfstack,--no-whole-archive -Wl,--no-whole-archive -lrt -lm -ldl -lcrypto -pthread -lnuma -lrte_bus_vdev"
fi


AC_CHECK_MEMBERS([struct stat.st_mtim.tv_nsec])

# check where makedev is defined
AC_HEADER_MAJOR

#output
AC_CONFIG_FILES([Makefile]
                [doc/Makefile]
                [examples/Makefile]
                [include/Makefile]
                [lib/Makefile]
                [mount/Makefile]
                [nfs/Makefile]
                [nfs4/Makefile]
                [nlm/Makefile]
                [nsm/Makefile]
                [portmap/Makefile]
                [rquota/Makefile]
                [tests/Makefile]
                [utils/Makefile]
               )

AC_OUTPUT([libnfs.pc])
