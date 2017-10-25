<h3><?php print $host_name; ?></h3>
<div class="error_log">
  <?php if ($error_log): ?>
  <textarea readonly rows="50" cols="140"><?php print htmlspecialchars($error_log); ?></textarea>
  <?php else: ?>
  <em>No error_log record exists for <?php print $host_name; ?>.</em>
  <?php endif; ?>
</div>