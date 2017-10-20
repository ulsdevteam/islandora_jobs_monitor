<div class="master" id="<?php print $host_record['hostname']; ?>">
  <?php unset($host_record['hostname']); ?>
  <?php foreach ($host_record as $key => $value): ?>
    <div class="div_label"><?php print str_replace("_", " ", ucwords($key)); ?></div>
    <div class="div_value"><?php print $value; ?></div>
    <br class="clearfix">
  <?php endforeach; ?>
</div>