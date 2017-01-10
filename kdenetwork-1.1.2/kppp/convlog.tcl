#!/usr/bin/tclsh

proc err { msg } {
    global argv0

    puts stderr "[lindex $argv0 0]: $msg"
}

foreach i [glob -nocomplain ~/.kde/share/apps/kppp/Log/*-199?] {

    if {[catch { set fin [open $i "r"] }]} {
	err "cannot open $i for reading"
	continue
    }

    if {[catch { set fout [open ${i}.log "a"] }]} {
	err "cannot open ${i}.log for writing"
	continue
    }    

    puts -nonewline "converting $i... "
    flush stdout
    
    set PHASE 1
    while {[eof $fin] == 0} {
	gets $fin line

	if {[regexp {(.*:.*:.*):.*:(.*):.*} $line s s1 s2]} {
	    set date [clock scan $s1]
	    if {$PHASE == 2} {
		# newline
		puts $fout ""
	    }
	    puts -nonewline $fout "$date:$s2"
	    set PHASE 2
	} else {
	    set PHASE 1
	    if {[regexp {(.*:.*:.*):} $line s s1]} {
		set date [clock scan $s1]
		
		gets $fin line1
		gets $fin line2
		regexp {.*:\ *([0-9.]+)\ *(.*)} $line1 s s1 s2
		regexp {.*:\ *([0-9.]+)\ *(.*)} $line2 s s3 s4
		puts $fout ":$s2:$date:$s1:$s3:-1:-1"
	    }
	}
    }
    close $fin
    close $fout

    # remove duplicate entries
    if {[catch { exec cat ${i}.log | sort -n | uniq | egrep {^[0-9]} > ${i}.log.new} ret]} {
	err "cannot sort ${i}.log $ret"
    } else {
	catch { exec mv ${i}.log.new ${i}.log }
    }

    catch {
	exec chmod 600 ${i}.log
    }

    puts "done"
}