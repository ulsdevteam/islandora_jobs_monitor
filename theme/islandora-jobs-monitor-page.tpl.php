<div class="masters">
  <label>Master/s</label>
  <?php foreach ($themed_hosts['master'] as $themed_host): ?>
    <?php echo $themed_host;?>
  <?php endforeach; ?>
</div>

<div class="workers">
  <label>Worker/s</label>
  <?php foreach ($themed_hosts['workers'] as $themed_host): ?>
    <?php echo $themed_host;?>
  <?php endforeach; ?>
</div>

<div class="monitor_report"><?php print $current_report; ?></div>
