<html lang="en">
<head>
<meta charset="UTF-8">
</head>
<body style="font-family: Arial, sans-serif; line-height: 1.55;">

<h1>⏳ Time Travelling File System</h1>

<p>
A C++-based <strong>in-memory versioned file system</strong> featuring snapshots, branching, rollbacks, history tracking, 
and heap-based metadata queries. Designed to mimic the core behavior of a lightweight version control system.
</p>

<hr>

<h2>📌 Overview</h2>
<p>
This project implements a version-controlled file system where each file is represented as a 
<strong>tree of versions</strong>. The system supports:
</p>

<ul>
  <li>Snapshots</li>
  <li>Rollbacks</li>
  <li>Branching/version trees</li>
  <li>Recency and size queries via heaps</li>
  <li>History traversal</li>
</ul>

<p>
Custom <strong>Hash Maps</strong>, <strong>Heaps</strong>, and <strong>Trees</strong> are implemented from scratch—no STL hashing or priority queues.
</p>

<hr>

<h2>🧱 Data Structures</h2>

<h3>📂 TreeNode</h3>
<p>Represents one version of a file.</p>
<ul>
  <li>version_id</li>
  <li>content</li>
  <li>message (snapshot message)</li>
  <li>created_timestamp</li>
  <li>snapshot_timestamp</li>
  <li>parent pointer</li>
  <li>children vector (branching)</li>
</ul>

<h3>📦 HashMap</h3>
<ul>
  <li>Maps <code>version_id → TreeNode*</code></li>
  <li>Uses linked lists for chaining</li>
  <li>Supports insert, search, delete</li>
</ul>

<h3>📁 File</h3>
<p>Represents a complete version tree.</p>
<ul>
  <li>root (version 0)</li>
  <li>active_version pointer</li>
  <li>total_versions</li>
  <li>versionMap (custom hash map)</li>
</ul>

Supports: <strong>READ, INSERT, UPDATE, SNAPSHOT, ROLLBACK, HISTORY</strong>.

<h3>🧩 HeapNode</h3>
Metadata used inside heaps:
<ul>
  <li>file_name</li>
  <li>filePtr</li>
  <li>last_modified</li>
  <li>total_versions</li>
  <li>index in heap array</li>
</ul>

<h3>🗂 CustomMap</h3>
<ul>
  <li>Maps filename → HeapNode*</li>
  <li>Used to update heap entries efficiently</li>
</ul>

<h3>🔺 MaxHeap</h3>
<p>Used for <strong>RECENT_FILES</strong> and <strong>BIGGEST_TREES</strong> queries.</p>
<ul>
  <li>Ordered by last_modified</li>
  <li>Ordered by total_versions (for biggest files)</li>
  <li>Supports insertOrUpdate, getMax, print</li>
</ul>

<h3>🖥 FileSystem</h3>
<ul>
  <li>Stores all files</li>
  <li>Tracks via heaps + custom maps</li>
  <li>Exposes command-level operations</li>
</ul>

<hr>

<h2>⚙️ Features</h2>

<h3>1. CREATE &lt;filename&gt;</h3>
<ul>
  <li>Creates a new file with root version 0</li>
  <li>Root is immediately snapshotted with message <em>"Initial Version"</em></li>
</ul>

<h3>2. READ &lt;filename&gt;</h3>
<p>Prints content of the active version.</p>

<h3>3. INSERT &lt;filename&gt; &lt;content&gt;</h3>
<ul>
  <li>Appends content to active version</li>
  <li>If active version is snapshotted → create new child</li>
</ul>

<h3>4. UPDATE &lt;filename&gt; &lt;content&gt;</h3>
<ul>
  <li>Replaces content of active version</li>
  <li>If snapshotted → creates new child version</li>
</ul>

<h3>5. SNAPSHOT &lt;filename&gt; &lt;message&gt;</h3>
<ul>
  <li>Marks active version as immutable</li>
  <li>Stores snapshot message + timestamp</li>
</ul>

<h3>6. ROLLBACK &lt;filename&gt; [versionID]</h3>
<ul>
  <li>If versionID given → direct jump</li>
  <li>If none → go to parent version</li>
  <li>Edge cases handled safely</li>
</ul>

<h3>7. HISTORY &lt;filename&gt;</h3>
<p>Shows all snapshot versions from root → active version.</p>

<h3>8. BIGGEST_TREES &lt;num&gt;</h3>
<p>Shows top files with largest number of versions.</p>

<h3>9. RECENT_FILES &lt;num&gt;</h3>
<p>Shows most recently modified files.</p>

<hr>

<h2>🛠 Compilation</h2>

<pre>
g++ LongAssignment.cpp -o LongAssignment
</pre>

<h2>▶️ Running</h2>

<pre>
./LongAssignment
</pre>

<hr>

<h2>❗ Error Handling</h2>

<ul>
  <li>Check file existence before every operation</li>
  <li>Safe messages for invalid reads/snapshots</li>
  <li>Heap boundary checks</li>
  <li>Rollback validity checking</li>
</ul>

<p>No crashes—errors are printed cleanly.</p>

<hr>

<h2>⌨️ Input Format</h2>

<p>Commands from terminal:</p>

<pre>
CREATE file1
INSERT file1 Good 
INSERT file1 _Morning
SNAPSHOT file1 this is the first snapshot after version 0 snapshot.
CREATE file2
INSERT file2 Good Evening
UPDATE file2 Good Night
SNAPSHOT file2 Version 1 is now a snapshot
INSERT file2 OK BYE NOW

READ file1
READ file2

HISTORY file1
HISTORY file2

ROLLBACK file2 1
READ file2

ROLLBACK file1
READ file1

BIGGEST_TREES 2
RECENT_FILES 2
</pre>

<hr>

</body>
</html>
